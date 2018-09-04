#include "../include/RuleParser.hpp"
#include "../parserlib/source/parserlib.hpp"
#include "../include/RuleParserAST.hpp"
#include "../include/Rete.hpp"

#include <map>
#include <tuple>
#include <utility>
#include <algorithm>

#include <iostream>

namespace rete {
namespace pl = parserlib;

// fwd decl helper method
void construct(ast::Rule&, Network&);

bool RuleParser::parseRules(const std::string& rulestring, Network& network)
{

    /**
        The pl::rules defined in this section implement the EBNF structure which the rules must
        follow. The parserlib constructs an AST by following these rules, and from this AST we can
        create the rete structure.
    */

    // special character groups
    pl::rule endl   = pl::nl(pl::expr("\r\n") | "\n\r" | '\n' | '\r');
    pl::rule ws     = *(pl::set(" \t") | endl);
    pl::rule alpha  = pl::range('a', 'z') | pl::range('A', 'Z');
    pl::rule num    = pl::range('0', '9');
    pl::rule alphanum = alpha | num;
    // // escape-chars, e.g. \t, \n, \\, \", ...
    pl::rule echar = pl::expr('\\') >> (pl::set("tbnrf\"\'\\"));

    /**
        NOTE: logical checks do *not* move the cursor forward through the string, so e.g. the rule
                !pl::expr('a') >> pl::any()
              only processes exactly one character, at first making sure it is not an 'a' and
              accepting anything afterwards -- so its "anything but 'a'". You could also say
                !pl::expr('a') >> alphanum
              for "anything alphanumerical, except 'a'".
    */
    // iri: anything between <> with some exceptions
    pl::rule iriref =  pl::expr('<') >>
                            *(!(pl::set("<>\"{}|^`\\") | pl::range('\x00', '\x20')) >> pl::any()) >>
                        pl::expr('>')
                      ;


    // string literal in quotes: some unallowed things, but escaped-chars are okay
    // NOTE: I have no clue what \x22, \x5C etc are, I partly followed this description: https://www.w3.org/TR/n-triples/#n-triples-grammar
    pl::rule stringliteralquote =
            pl::expr('\"') >>
                *( (!pl::set("\x22\x5C\xA\xD") >> pl::any())
                    | echar ) >>
            pl::expr('\"');

    pl::rule langtag = pl::expr('@') >> +alpha >> *(pl::expr('-') >> +alphanum);
    pl::rule literal = stringliteralquote >> -(pl::expr("^^") >> iriref | langtag);

    pl::rule blank_node_label = pl::expr("_:") >> *alphanum;
    pl::rule variable = pl::expr('?') >> +alphanum;

    pl::rule subject =      pl::term(variable | iriref | blank_node_label);
    pl::rule predicate =    pl::term(variable | iriref);
    pl::rule object =       pl::term(variable | iriref | blank_node_label | literal);

    pl::rule triple = pl::expr('(')
                        >> subject
                        >> predicate
                        >> object
                        >> pl::expr(')');

    pl::rule triples = triple >> *(pl::expr(',') >> triple);
    pl::rule rule = pl::expr('[') >>
                        triples >> pl::expr('-') >> pl::expr('>') >> triples >>
                    pl::expr(']');

    // the main thing to parse: rules. Many of them.
    pl::rule rules = +rule;

    /**
        The pl::ast<...> objects create the connection between the user defined ast-classes (i.e.,
        our ast-classes, e.g. ast::String, ast::Triple, ast::Rule, ...) with the grammar. The
        classes have to follow the same structure as the grammar, so e.g. our grammar rule
        "triples" states that it is essentially multiple "triple"s separated by comma, so the
        ast::Triples class only defines a member variable of type pl::ast_list<ast::Triple>, which
        gets filles automatically by the parser. How? ~~magic~~
    */
    pl::ast<ast::TripleElement> ast_subject(subject);
    pl::ast<ast::TripleElement> ast_predicate(predicate);
    pl::ast<ast::TripleElement> ast_object(object);

    pl::ast<ast::Triple> ast_triple(triple);
    pl::ast<ast::Triples> ast_triples(triples);
    pl::ast<ast::Rule> ast_rule(rule);
    pl::ast<ast::Rules> ast_rules(rules);


    pl::input input;
    input.insert(input.begin(), rulestring.begin(), rulestring.end());

    pl::error_list el;
    ast::Rules* r = nullptr;
    pl::parse(input, rules, ws, el, r);

    if (r)
    {
        std::cout << "success!" << std::endl;
        for (auto rule : r->rules_.objects())
        {
            std::cout << "rule:" << std::endl;
            for (auto c : rule->conditions_->triples_.objects())
            {
                std::cout << *c << " ";
            }
            std::cout << "-> ";
            for (auto e : rule->effects_->triples_.objects())
            {
                std::cout << *e << " ";
            }
            std::cout << std::endl;
        }
    }
    else
    {
        std::cout << "failed" << std::endl;
        for (auto e : el)
        {
            std::cout << "Error: (" << e.m_begin.m_line << ", " << e.m_begin.m_col << ") type " << e.m_type << " -- " << e << std::endl;
        }
        return false;
    }



    /**
        The parsing is done, now we need to construct nodes in the network.
    */
    for (auto rule : r->rules_.objects())
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
        TripleAlpha::Ptr check(new TripleAlpha(Triple::SUBJECT, triple.subject_->value_));
        current = implementAlphaNode(check, current);
    }
    if (!triple.predicate_->isVariable())
    {
        TripleAlpha::Ptr check(new TripleAlpha(Triple::PREDICATE, triple.predicate_->value_));
        current = implementAlphaNode(check, current);
    }
    if (!triple.object_->isVariable())
    {
        TripleAlpha::Ptr check(new TripleAlpha(Triple::OBJECT, triple.object_->value_));
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
            if (triple.subject_->value_ == triple.predicate_->value_)
            {
                TripleConsistency::Ptr check(new TripleConsistency(Triple::SUBJECT, Triple::PREDICATE));
                current = implementAlphaNode(check, current);
            }
        }
        // sub & obj
        if (triple.object_->isVariable())
        {
            if (triple.subject_->value_ == triple.object_->value_)
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
            if (triple.predicate_->value_ == triple.object_->value_)
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

    auto conditionTriples = rule.conditions_->triples_.objects();
    auto conditionIterator = conditionTriples.begin();

    // 1. first condition
    AlphaMemory::Ptr condition = constructAlphaMemory(**conditionIterator, net);
    // remember variables
    if ((**conditionIterator).subject_->isVariable()) variableLocation[(**conditionIterator).subject_->value_] = {0, Triple::SUBJECT};
    if ((**conditionIterator).predicate_->isVariable()) variableLocation[(**conditionIterator).predicate_->value_] = {0, Triple::PREDICATE};
    if ((**conditionIterator).object_->isVariable()) variableLocation[(**conditionIterator).object_->value_] = {0, Triple::OBJECT};

    std::cout << "variables:" << std::endl;
    for (auto v : variableLocation)
    {
        std::cout << v.first << " -- " << v.second.first << "," << v.second.second << std::endl;
    }

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
        std::cout << "construction from " << **conditionIterator << std::endl;
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
            auto var = variableLocation.find(conditionTriple.subject_->value_);
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
            auto var = variableLocation.find(conditionTriple.predicate_->value_);
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
            auto var = variableLocation.find(conditionTriple.object_->value_);
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
            variableLocation[conditionTriple.subject_->value_] = {conditionCount, Triple::SUBJECT};
        }
        if (conditionTriple.predicate_->isVariable())
        {
            variableLocation[conditionTriple.predicate_->value_] = {conditionCount,
                                                                    Triple::PREDICATE};
        }
        if (conditionTriple.object_->isVariable())
        {
            variableLocation[conditionTriple.object_->value_] = {conditionCount, Triple::OBJECT};
        }

        ++conditionCount;
    } // end loop over condition triples

}





} /* rete */
