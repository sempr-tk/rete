#include "../rete-core/ReteCore.hpp"
#include "../rete-rdf/ReteRDF.hpp"

#include "RuleParser.hpp"
#include "RuleParserAST.hpp"
#include "TripleConditionBuilder.hpp"
#include "TripleEffectBuilder.hpp"
#include "MathBuiltinBuilder.hpp"
#include "UtilBuiltinBuilder.hpp"

#include <map>
#include <tuple>
#include <utility>
#include <algorithm>
#include <sstream>
#include <iostream>

namespace rete {
namespace peg = pegmatite;

RuleParser::RuleParser()
{
    // registerNodeBuilder(std::unique_ptr<TripleConditionBuilder>(new TripleConditionBuilder()));
    registerNodeBuilder<TripleConditionBuilder>();
    registerNodeBuilder<TripleEffectBuilder>();
    registerNodeBuilder<builtin::MathBuiltinBuilder<builtin::Sum>>();
    registerNodeBuilder<builtin::MathBuiltinBuilder<builtin::Mul>>();
    registerNodeBuilder<builtin::MathBuiltinBuilder<builtin::Div>>();
    registerNodeBuilder<builtin::CompareNodeBuilder<builtin::Compare::LT>>();
    registerNodeBuilder<builtin::CompareNodeBuilder<builtin::Compare::LE>>();
    registerNodeBuilder<builtin::CompareNodeBuilder<builtin::Compare::EQ>>();
    registerNodeBuilder<builtin::CompareNodeBuilder<builtin::Compare::NEQ>>();
    registerNodeBuilder<builtin::CompareNodeBuilder<builtin::Compare::GE>>();
    registerNodeBuilder<builtin::CompareNodeBuilder<builtin::Compare::GT>>();
    registerNodeBuilder<builtin::PrintNodeBuilder>();
}

void RuleParser::registerNodeBuilder(std::unique_ptr<NodeBuilder> builder)
{
    std::map<std::string, std::unique_ptr<NodeBuilder>>* map = &conditionBuilders_;
    if (builder->builderType() == NodeBuilder::EFFECT) map = &effectBuilders_;

    std::string t = builder->type();
    if (map->find(t) != map->end())
    {
        std::cout << "Already registered a NodeBuilder for the same effect/condition!" << std::endl;
        std::cout << builder->type() << ", "
                  << (builder->builderType() == NodeBuilder::EFFECT ? "effect" : "condition")
                  << std::endl;
        throw std::exception(); // already registerd a builder for this type
    }

    (*map)[t] = std::move(builder);
}


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
        /*
            Build a map of used URI-prefixes.
        */
        std::cout << "successfully parsed the rules." << std::endl;
        // std::cout << input.getString() << std::endl;
        std::cout << "Prefixes: " << root->prefixes_.size() << std::endl;
        std::map<std::string, std::string> prefixes;
        for (auto& pre : root->prefixes_)
        {
            std::cout << pre->name_ << " --> " << pre->uri_ << std::endl;
            const std::string& uri = pre->uri_;
            prefixes[pre->name_ + ":"] = uri.substr(1, uri.size() - 2); // trim <>
        }

        /*
            Preprocess: Substitute prefixes in the arguments of conditions and effects
        */
        for (auto& rule : root->rules_)
        {
            std::cout << "Rule: ";
            if (rule->name_) std::cout << *rule->name_;
            std::cout << std::endl;
            for (auto& condition : rule->conditions_)
            {
                std::cout << "  Condition: "
                          << (condition->isNoValue() ? "-" : "+")
                          << " "
                          << condition->type()
                          << std::endl;
                for (auto& arg : condition->args_)
                {
                    arg->substitutePrefixes(prefixes);
                    std::cout << "    " << *arg << std::endl;
                }
            }

            for (auto& effect : rule->effects_)
            {
                std::cout << "  Effect:" << std::endl;
                for (auto& arg : effect->args_)
                {
                    arg->substitutePrefixes(prefixes);
                    std::cout << "    " << *arg << std::endl;
                }
            }
        }

        /**
            Construct the network
        */
        for (auto& rule : root->rules_)
        {
            construct(*rule, network);
        }

        return true;
    }

    return false;
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

    if (it != candidates.end())
    {
        std::cout << "Reusing AlphaNode " << (*it)->getDOTId() << std::endl;
        alpha = *it;
    }
    else
    {
        std::cout << "Adding AlphaNode " << alpha->getDOTId() << " beneath " << parent->getDOTId()  << std::endl;
        parent->addChild(alpha);
    }

    return alpha;
}


/**
    Try to find an equivalent node at the parent. If not found add the given node. Returns the node
    which is connected to the parent after this operation.
*/
BetaNode::Ptr implementBetaNode(BetaNode::Ptr node, BetaNode::Ptr parentBeta, AlphaNode::Ptr parentAlpha)
{
    BetaNode::Ptr beta = node;

    std::vector<BetaNode::Ptr> candidates;
    parentBeta->getBetaMemory()->getChildren(candidates);
    auto it = std::find_if(candidates.begin(), candidates.end(),
        [beta] (BetaNode::Ptr other) -> bool
        {
            return *beta == *other;
        }
    );

    // reuse or connect new, if the same join was found in the beta parent that connects to the wanted alpha
    AlphaMemory::Ptr pamem = (parentAlpha ? parentAlpha->getAlphaMemory() : nullptr);

    if (it != candidates.end() && (*it)->getParentAlpha() == pamem)
    {
        std::cout << "Reusing BetaNode " << (*it)->getDOTId() << std::endl;
        beta = *it;
    }
    else
    {
        std::cout << "Adding BetaNode " << beta->getDOTId() << " beneath " << parentBeta->getDOTId() << " and " << (parentAlpha ? parentAlpha->getDOTId() : " 0x0") << std::endl;
        BetaNode::connect(beta, parentBeta->getBetaMemory(),
                 (parentAlpha ? parentAlpha->getAlphaMemory() : nullptr));
    }
    return beta;
}


/**
    Find or create an AlphaBetaAdapter under the given node
*/
AlphaBetaAdapter::Ptr getAlphaBeta(AlphaMemory::Ptr amem)
{
    AlphaBetaAdapter::Ptr adapter(new AlphaBetaAdapter());

    std::vector<BetaNode::Ptr> amemChildren;
    amem->getChildren(amemChildren);

    auto it = std::find_if(amemChildren.begin(), amemChildren.end(),
        [adapter](BetaNode::Ptr other) -> bool
        {
            return *adapter == *other;
        }
    );

    if (it != amemChildren.end()) adapter = std::dynamic_pointer_cast<AlphaBetaAdapter>(*it);
    else BetaNode::connect(adapter, nullptr, amem);

    return adapter;
}


/**
    Implement the rule in the given network.
*/
void RuleParser::construct(ast::Rule& rule, Network& net) const
{
    // keep track of the last implemented beta node
    BetaNode::Ptr currentBeta = nullptr;

    // remember already bound variables
    std::map<std::string, Accessor::Ptr> bindings;

    // construct all the conditions
    for (auto& condition : rule.conditions_)
    {
        // find the correct builder
        auto bIt = conditionBuilders_.find(condition->type());
        if (bIt == conditionBuilders_.end())
        {
            std::cout << "no builder for type " << condition->type() << std::endl;
            throw std::exception(); // no builder for this type
        }
        auto& builder = *bIt->second;

        // create an argument list. The ast::Arguments are consumed.
        // NOTE: When constructing an alpha node, variables are always unbound, though they might have been used before in other alpha nodes of the same rule! Bindings only happen in the beta network, when they are given through the context of the sub-match. So, if we are constructing an AlphaNode, create the arguments with an empty bindings-map.
        ArgumentList args;
        for (auto& astArg : condition->args_)
        {
            std::map<std::string, Accessor::Ptr> emptyBindings;
            std::map<std::string, Accessor::Ptr>* usedBindings = &emptyBindings;
            if (builder.builderType() != NodeBuilder::ALPHA)
                usedBindings = &bindings;

            Argument arg = Argument::createFromAST(std::move(astArg), *usedBindings);
            args.push_back(std::move(arg));
        }

        // Only relevant in case of an alpha node: Remember all the variables given to the builder that were previously bound by another node. These are the ones we need to create a join for in the beta network.
        std::vector<size_t> joinVars;
        for (size_t argIndex = 0; argIndex < args.size(); argIndex++)
        {
            // for the nodebuilder it will be seen as an unbound variable
            if (args[argIndex].isVariable() && !args[argIndex].getAccessor())
            {
                // but it was bound before
                auto prevBound = bindings.find(args[argIndex].getVariableName());
                if (prevBound != bindings.end() && prevBound->second != nullptr)
                {
                    joinVars.push_back(argIndex);
                }
            }
        }

        if (builder.builderType() == NodeBuilder::ALPHA)
        {
            // - create and implement the chain of alpha nodes
            std::vector<AlphaNode::Ptr> anodes;
            builder.buildAlpha(args, anodes);

            AlphaNode::Ptr currentAlpha = net.getRoot();
            for (auto alpha : anodes)
            {
                currentAlpha = implementAlphaNode(alpha, currentAlpha);
            }
            currentAlpha->initAlphaMemory(); // no-op if already initialized.

            // - if there has been a beta node before, create a join
            // - else create an AlphaBetaAdapter
            if (currentBeta)
            {
                GenericJoin::Ptr join(new GenericJoin());
                if (condition->isNoValue())
                {
                    // condition is negated with "noValue" modifier.
                    // --> negate the join!
                    join->setNegative(true);
                }

                // create one check for every variable that was previously unbound
                for (auto jv : joinVars)
                {
                    Argument& arg = args[jv];
                    std::cout << "adding join check on variable " << arg.getVariableName() << std::endl;
                    if (!arg.getAccessor())
                    {
                        // the node did not bind the variable it was given? this is an error!
                        throw std::exception();
                    }

                    Accessor::Ptr beta(bindings[arg.getVariableName()]->clone());
                    Accessor::Ptr alpha(arg.getAccessor()->clone());

                    // add the check to the join
                    join->addCheck(beta, alpha);
                }

                // add the join.
                currentBeta = implementBetaNode(join, currentBeta, currentAlpha);
            }
            else
            {
                if (condition->isNoValue())
                {
                    throw std::exception(); // first alpha condition must not be negated!
                }

                BetaNode::Ptr alphabeta = getAlphaBeta(currentAlpha->getAlphaMemory());
                std::cout << "Adding AlphaBetaNode " << alphabeta << " beneath <nullptr> and " << currentAlpha << std::endl;

                currentBeta = alphabeta;
            }
        }
        else if (builder.builderType() == NodeBuilder::BUILTIN)
        {
            // rules *must* start with an alpha check
            if (!currentBeta) throw std::exception();

            // create and implement the builtin node
            Builtin::Ptr builtin = builder.buildBuiltin(args);
            currentBeta = implementBetaNode(builtin, currentBeta, nullptr);
        }


        // After constructing one condition, update the known variable bindings!
        // NOTE: In general, we want to override all bindings we can, so that we always have
        //       the latest accessor for every variable (if one occurs in multiple conditions).
        //       But be aware: If the condition is negated with "noValue", new variables are
        //       actually placeholders and cannot be bound -- there is "noValue" to bind them to.
        //       The NodeBuilder will create accessors nevertheless, which would lead to segfaults
        //       and undefined behavious, as we would e.g. try to cast an EmptyWME to a Triple.
        if (!condition->isNoValue())
        {
            for (auto& arg : args)
            {
                if (arg.isVariable())
                {
                    auto accessor = arg.getAccessor();
                    if (!accessor) throw std::exception(); // a NodeBuilder left a variable unbound!


                    bindings[arg.getVariableName()] = accessor;
                }
            }
        }

        /*
            Every time we created a beta node (join or builtin, or alphabeta), we need to increment
            the accessors afterwards. ASSUMPTION: The accessors bound to variables by the node
            builders are indexed at -1, so that we can just increment them all and get them to
            index 0, the first wme in the token chain.
        */
        for (auto binding : bindings)
        {
            ++(binding.second->index());
        }


    } // end loop over conditions


    //  create productions / effects
    for (auto& effect : rule.effects_)
    {
        // find the correct builder
        auto it = effectBuilders_.find(effect->type());
        if (it == effectBuilders_.end())
        {
            std::cout << "no effect-builder for type " << effect->type() << std::endl;
            throw std::exception();
        }

        // create an argument list. The ast::Arguments are consumed.
        ArgumentList args;
        for (auto& astArg : effect->args_)
        {
            Argument arg = Argument::createFromAST(std::move(astArg), bindings);
            // all args in the effects MUST be bound variables or constants!
            if (arg.isVariable() && !arg.getAccessor())
            {
                throw std::exception(); // unbound variable in rule effect
            }
            args.push_back(std::move(arg));
        }

        // create the production, encapsulate it in an AgendaNode, and add it to the current pattern results (beta memory)
        auto production = it->second->buildEffect(args);
        AgendaNode::Ptr agendaNode(new AgendaNode(production, net.getAgenda()));

        if (rule.name_) agendaNode->setName(*rule.name_);
        else            agendaNode->setName("");

        currentBeta->getBetaMemory()->addProduction(agendaNode);
    }

    // /**
    //     To construct the rule in the network, we do the following:
    //         1. Create the first condition in the alpha network
    //         2. Create an AlphaBetaAdapter, track the latest beta-memory
    //         3. While there are still conditions left:
    //             3.1 Create the next condition
    //             3.2 Create a join node between the latest beta memory and the new condition
    //             3.3 Update latest beta memory to be the one of the join node
    //             3.4 Update the set of known variables (with additions from 3.1)
    //         4. Add the consequences through ProductionNodes to the latest beta memory
    //
    //     In order to correctly create join nodes we need to keep track of the occurences of variable
    //     names. Therefore we keep a set with variable names, the condition index (to be used in the
    //     tokens in the partial matches in the rete) and the field (since the WMEs addressed by the
    //     condition index are triples with subject, predicate, object).
    // */

    //
    // // 4. add the consequences to the end of the condition-chain (the betaMemory we kept track of)

    //     // create the InferTriple production
    //     InferTriple::Ptr infer(new InferTriple(s, p, o));
    //     AgendaNode::Ptr inferNode(new AgendaNode(infer, net.getAgenda()));
    //     if (rule.name_)
    //         inferNode->setName(*rule.name_);
    //     else
    //         inferNode->setName("");
    //
    //     betaMemory->addProduction(inferNode);
    // }
}





} /* rete */
