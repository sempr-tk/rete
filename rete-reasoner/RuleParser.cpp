#include <rete-core/ReteCore.hpp>
#include <rete-rdf/ReteRDF.hpp>

#include "RuleParser.hpp"
#include "RuleParserAST.hpp"

#include <map>
#include <tuple>
#include <utility>
#include <algorithm>
#include <sstream>
#include <iostream>

namespace rete {
namespace peg = pegmatite;

// fwd decl helper method
void construct(ast::Rule&, Network&);

bool RuleParser::parseRules(const std::string& rulestring_pre, Network& network)
{
    // preprocessing: remove comment-lines.
    std::stringstream ss(rulestring_pre);
    std::string line;
    std::string rulestring;
    while (std::getline(ss, line))
    {
        if (line.empty() || line.at(0) == '#')
        {
            continue;
        }
        rulestring += line + "\n";
    }
    std::cout << "parsing rules:" << std::endl;
    std::cout << rulestring << std::endl;

    std::unique_ptr<ast::Rules> root = nullptr;
    peg::StringInput input(std::move(rulestring));
    this->parse(input, g.rules, g.ws, peg::defaultErrorReporter, root);

    if (root)
    {
        std::cout << "success!" << std::endl;
        std::cout << "Prefixes:" << std::endl;
        std::map<std::string, std::string> prefixes;

        for(auto& pre : root->prefixes_)
        {
            std::cout << *pre->name_ << " --> " << *pre->uri_ << std::endl;

            std::string& uri = *pre->uri_;
            prefixes[*pre->name_ + ":"] = uri.substr(1, uri.size() - 2); // trim <>
        }


        // important: preprocess the rules by substituting the prefixes
        for (auto& rule : root->rules_)
        {
            std::cout << "rule:" << std::endl;
            if (rule->name_) std::cout << *rule->name_ << " ";
            for (auto& c : rule->conditions_->triples_)
            {
                c->substitutePrefixes(prefixes);
                std::cout << *c << " ";
            }
            std::cout << "-> ";
            for (auto& e : rule->effects_->triples_)
            {
                e->substitutePrefixes(prefixes);
                std::cout << *e << " ";
            }
            std::cout << std::endl;

        }
    }
    else
    {
        std::cout << "failed" << std::endl;
        return false;
    }



    /**
        The parsing is done, now we need to construct nodes in the network.
    */
    for (auto& rule : root->rules_)
    {
        construct(*rule, network);
    }

    return true;
}


/**
    Try to find an equivalent node at the parent. If not found add the given node. Returns the node
    which is connected to the parent after this operation.
*/
AlphaNode::Ptr implementAlphaNode(AlphaNode::Ptr alpha, AlphaNode::Ptr parent)
{
    std::vector<AlphaNode::Ptr> candidates;
    parent->getChildren(candidates);
    auto it = std::find_if(candidates.begin(), candidates.end(),
        [alpha](AlphaNode::Ptr other) -> bool
        {
            return *other == *alpha;
        }
    );

    if (it != candidates.end()) alpha = *it;
    else parent->addChild(alpha);

    return alpha;
}



AlphaMemory::Ptr constructAlphaMemory(ast::Triple& triple, Network& net)
{
    AlphaNode::Ptr current = net.getRoot();
    // ---------------------------------------------
    // 1. static checks for the non-variable parts.
    // ---------------------------------------------
    if (!triple.subject_->isVariable())
    {
        TripleAlpha::Ptr check(new TripleAlpha(Triple::SUBJECT, *triple.subject_->value_));
        current = implementAlphaNode(check, current);
    }
    if (!triple.predicate_->isVariable())
    {
        TripleAlpha::Ptr check(new TripleAlpha(Triple::PREDICATE, *triple.predicate_->value_));
        current = implementAlphaNode(check, current);
    }
    if (!triple.object_->isVariable())
    {
        TripleAlpha::Ptr check(new TripleAlpha(Triple::OBJECT, *triple.object_->value_));
        current = implementAlphaNode(check, current);
    }
    // ------------------------------------------------
    // 2. consistency checks for variable with equal name
    // ------------------------------------------------
    if (triple.subject_->isVariable())
    {
        // sub & pred
        if (triple.predicate_->isVariable())
        {
            if (*triple.subject_->value_ == *triple.predicate_->value_)
            {
                TripleConsistency::Ptr check(new TripleConsistency(Triple::SUBJECT, Triple::PREDICATE));
                current = implementAlphaNode(check, current);
            }
        }
        // sub & obj
        if (triple.object_->isVariable())
        {
            if (*triple.subject_->value_ == *triple.object_->value_)
            {
                TripleConsistency::Ptr check(new TripleConsistency(Triple::SUBJECT, Triple::OBJECT));
                current = implementAlphaNode(check, current);
            }
        }
    }
    if (triple.predicate_->isVariable())
    {
        // pred & obj
        if (triple.object_->isVariable())
        {
            if (*triple.predicate_->value_ == *triple.object_->value_)
            {
                TripleConsistency::Ptr check(new TripleConsistency(Triple::PREDICATE, Triple::OBJECT));
                current = implementAlphaNode(check, current);
            }
        }
    }

    // ---------------------------------------------
    // 3. init alpha memory
    // ---------------------------------------------
    current->initAlphaMemory();
    return current->getAlphaMemory();
}


/**
    Implement the rule in the given network.
*/
void construct(ast::Rule& rule, Network& net)
{
    /**
        To construct the rule in the network, we do the following:
            1. Create the first condition in the alpha network
            2. Create an AlphaBetaAdapter, track the latest beta-memory
            3. While there are still conditions left:
                3.1 Create the next condition
                3.2 Create a join node between the latest beta memory and the new condition
                3.3 Update latest beta memory to be the one of the join node
                3.4 Update the set of known variables (with additions from 3.1)
            4. Add the consequences through ProductionNodes to the latest beta memory

        In order to correctly create join nodes we need to keep track of the occurences of variable
        names. Therefore we keep a set with variable names, the condition index (to be used in the
        tokens in the partial matches in the rete) and the field (since the WMEs addressed by the
        condition index are triples with subject, predicate, object).
    */
    typedef std::pair<int, Triple::Field> index_t;
    std::map<std::string, index_t> variableLocation;

    auto& conditionTriples = rule.conditions_->triples_;
    auto conditionIterator = conditionTriples.begin();

    // 1. first condition
    AlphaMemory::Ptr condition = constructAlphaMemory(**conditionIterator, net);
    // remember variables
    if ((**conditionIterator).subject_->isVariable()) variableLocation[*(**conditionIterator).subject_->value_] = {0, Triple::SUBJECT};
    if ((**conditionIterator).predicate_->isVariable()) variableLocation[*(**conditionIterator).predicate_->value_] = {0, Triple::PREDICATE};
    if ((**conditionIterator).object_->isVariable()) variableLocation[*(**conditionIterator).object_->value_] = {0, Triple::OBJECT};

    // std::cout << "variables:" << std::endl;
    // for (auto v : variableLocation)
    // {
    //     std::cout << v.first << " -- " << v.second.first << "," << v.second.second << std::endl;
    // }

    ++conditionIterator;

    // 2. Adapter
    AlphaBetaAdapter::Ptr adapter(new AlphaBetaAdapter());
    std::vector<BetaNode::Ptr> amemChildren;
    condition->getChildren(amemChildren);
    auto it = std::find_if(amemChildren.begin(), amemChildren.end(),
        [adapter](BetaNode::Ptr other) -> bool
        {
            return *adapter == *other;
        }
    );
    if (it != amemChildren.end()) adapter = std::dynamic_pointer_cast<AlphaBetaAdapter>(*it);
    else BetaNode::connect(adapter, nullptr, condition);

    // 3. beta network
    BetaMemory::Ptr betaMemory = adapter->getBetaMemory();
    size_t conditionCount = 1;
    for (; conditionIterator != conditionTriples.end(); ++conditionIterator)
    {
        // std::cout << "construction from " << **conditionIterator << std::endl;
        // 3.1 create condtion
        condition = constructAlphaMemory(**conditionIterator, net);
        // 3.2 create join
        TripleJoin::Ptr join(new TripleJoin());
        // check for join-conditions.
        ast::Triple& conditionTriple = **conditionIterator;
        // -- subject?
        if (conditionTriple.subject_->isVariable())
        {
            // check if the variable is already known (--> bound.)
            auto var = variableLocation.find(*conditionTriple.subject_->value_);
            if (var != variableLocation.end())
            {
                // found it! --> add a check.
                size_t tokenIndex = conditionCount - (var->second.first + 1);
                TripleJoin::check_t check(tokenIndex, var->second.second, Triple::SUBJECT);
                join->addCheck(check);
            }
        }
        // -- predicate?
        if (conditionTriple.predicate_->isVariable())
        {
            // check if the variable is already known (--> bound.)
            auto var = variableLocation.find(*conditionTriple.predicate_->value_);
            if (var != variableLocation.end())
            {
                // found it! --> add a check.
                size_t tokenIndex = conditionCount - (var->second.first + 1);
                TripleJoin::check_t check(tokenIndex, var->second.second, Triple::PREDICATE);
                join->addCheck(check);
            }
        }
        // -- object?
        if (conditionTriple.object_->isVariable())
        {
            // check if the variable is already known (--> bound.)
            auto var = variableLocation.find(*conditionTriple.object_->value_);
            if (var != variableLocation.end())
            {
                // found it! --> add a check.
                /**
                    NOTE: The token index must be inverted: The TripleJoin-Node follows the token n-steps from the end backwards. So when we want to address the last condition in the token we need tokenIndex 0, and if we want the first we need the tokens length-1 (if length = 1 index is 0). In general: i'th WME in token has index (tokenlength - (i+1)), where tokenlength is the number of conditions we have already processed --> conditionCount;
                */
                size_t tokenIndex = conditionCount - (var->second.first + 1);
                TripleJoin::check_t check(tokenIndex, var->second.second, Triple::OBJECT);
                join->addCheck(check);
            }
        }

        // check if an equivalent join already exists:
        std::vector<BetaNode::Ptr> joinCandidates;
        betaMemory->getChildren(joinCandidates);
        auto it = std::find_if(joinCandidates.begin(), joinCandidates.end(),
            [join] (BetaNode::Ptr other) -> bool
            {
                return *join == *other;
            }
        );

        // reuse or connect new
        if (it != joinCandidates.end() && (*it)->getParentAlpha() == condition)
            join = std::static_pointer_cast<TripleJoin>(*it);
        else BetaNode::connect(join, betaMemory, condition);

        // 3.3
        betaMemory = join->getBetaMemory();
        // 3.4
        if (conditionTriple.subject_->isVariable())
        {
            variableLocation[*conditionTriple.subject_->value_] = {conditionCount, Triple::SUBJECT};
        }
        if (conditionTriple.predicate_->isVariable())
        {
            variableLocation[*conditionTriple.predicate_->value_] = {conditionCount,
                                                                    Triple::PREDICATE};
        }
        if (conditionTriple.object_->isVariable())
        {
            variableLocation[*conditionTriple.object_->value_] = {conditionCount, Triple::OBJECT};
        }

        ++conditionCount;
    } // end loop over condition triples


    // 4. add the consequences to the end of the condition-chain (the betaMemory we kept track of)
    auto& consequences = rule.effects_->triples_;
    for (auto& consequence : consequences)
    {
        auto& sub = consequence->subject_;
        auto& pred = consequence->predicate_;
        auto& obj = consequence->object_;

        InferTriple::ConstructHelper s, p, o;

        if (sub->isVariable())
        {
            auto it = variableLocation.find(*sub->value_);
            if (it == variableLocation.end()) throw std::exception(); // unbound var in effect
            size_t tokenIndex = conditionCount - (it->second.first + 1);
            s.init(tokenIndex, it->second.second);
        }
        else
        {
            s.init(*sub->value_);
        }

        if (pred->isVariable())
        {
            auto it = variableLocation.find(*pred->value_);
            if (it == variableLocation.end()) throw std::exception(); // unbound var in effect
            size_t tokenIndex = conditionCount - (it->second.first + 1);
            p.init(tokenIndex, it->second.second);
        }
        else
        {
            p.init(*pred->value_);
        }

        if (obj->isVariable())
        {
            auto it = variableLocation.find(*obj->value_);
            if (it == variableLocation.end()) throw std::exception(); // unbound var in effect
            size_t tokenIndex = conditionCount - (it->second.first + 1);
            o.init(tokenIndex, it->second.second);
        }
        else
        {
            o.init(*obj->value_);
        }

        // create the InferTriple production
        InferTriple::Ptr infer(new InferTriple(s, p, o));
        AgendaNode::Ptr inferNode(new AgendaNode(infer, net.getAgenda()));
        if (rule.name_)
            inferNode->setName(*rule.name_);
        else
            inferNode->setName("");

        betaMemory->addProduction(inferNode);
    }
}





} /* rete */
