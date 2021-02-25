#include "../rete-core/ReteCore.hpp"
#include "../rete-rdf/ReteRDF.hpp"
#include "../rete-core/BetaBetaNode.hpp"
#include "../rete-core/NoValue.hpp"
#include "../rete-core/GroupBy.hpp"
#include "../rete-core/TokenGroupAccessor.hpp"
#include "../rete-core/builtins/MathBulk.hpp"

#include "RuleParser.hpp"
#include "RuleParserAST.hpp"
#include "TripleConditionBuilder.hpp"
#include "TripleEffectBuilder.hpp"
#include "MathBuiltinBuilder.hpp"
#include "SumBuiltinBuilder.hpp"
#include "MulBuiltinBuilder.hpp"
#include "DivBuiltinBuilder.hpp"
#include "MathBulkBuiltinBuilder.hpp"
#include "UtilBuiltinBuilder.hpp"
#include "TrueNodeBuilder.hpp"
#include "MakeSkolemBuilder.hpp"

#include "Exceptions.hpp"

#include <map>
#include <tuple>
#include <utility>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <memory>

namespace rete {

void error_reporter(const peg::InputRange& input, const std::string& error)
{
    std::cout << "An error occured!" << std::endl;
    std::cout << "    " << error << std::endl;
    std::cout << input.str() << std::endl;
    std::cout << input.start.line << ":" << input.start.col << std::endl;
    std::cout << input.finish.line << ":" << input.finish.col << std::endl;
    std::cout << "-----------------" << std::endl;
}


RuleParser::RuleParser()
{
    // registerNodeBuilder(std::unique_ptr<TripleConditionBuilder>(new TripleConditionBuilder()));
    registerNodeBuilder<TripleConditionBuilder>();
    registerNodeBuilder<TripleEffectBuilder>();
    /*
    registerNodeBuilder<builtin::MathBuiltinBuilder<builtin::Sum>>();
    registerNodeBuilder<builtin::MathBuiltinBuilder<builtin::Mul>>();
    registerNodeBuilder<builtin::MathBuiltinBuilder<builtin::Div>>();
    */
    registerNodeBuilder<builtin::SumBuiltinBuilder>("sum");
    registerNodeBuilder<builtin::MulBuiltinBuilder>("mul");
    registerNodeBuilder<builtin::DivBuiltinBuilder>("div");

    registerNodeBuilder<builtin::MathBulkBuiltinBuilder<builtin::SumBulk>>("SumBulk");
    registerNodeBuilder<builtin::MathBulkBuiltinBuilder<builtin::MulBulk>>("MulBulk");
    registerNodeBuilder<builtin::CountEntriesInGroupBuilder>();
    registerNodeBuilder<builtin::CompareNodeBuilder<builtin::Compare::LT>>();
    registerNodeBuilder<builtin::CompareNodeBuilder<builtin::Compare::LE>>();
    registerNodeBuilder<builtin::CompareNodeBuilder<builtin::Compare::EQ>>();
    registerNodeBuilder<builtin::CompareNodeBuilder<builtin::Compare::NEQ>>();
    registerNodeBuilder<builtin::CompareNodeBuilder<builtin::Compare::GE>>();
    registerNodeBuilder<builtin::CompareNodeBuilder<builtin::Compare::GT>>();
    registerNodeBuilder<builtin::PrintNodeBuilder>();
    registerNodeBuilder<builtin::PrintEffectNodeBuilder>();
    registerNodeBuilder<TrueNodeBuilder>();
    registerNodeBuilder<MakeSkolemBuilder>();
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
        throw std::runtime_error(
                std::string("Already registered a NodeBuilder for the ") +
                (builder->builderType() == NodeBuilder::EFFECT ? "effect " : "condition ") +
                builder->type()); // already registerd a builder for this type
    }

    (*map)[t] = std::move(builder);
}


std::vector<ParsedRule::Ptr> RuleParser::parseRules(const std::string& rulestring, Network& network)
{
#ifdef RETE_PARSER_VERBOSE
    std::cout << "parsing rules:" << std::endl;
    std::cout << rulestring << std::endl;
#endif
    std::string ruleStringCopy(rulestring);

    std::unique_ptr<ast::Rules> root = nullptr;
    peg::StringInput input(std::move(ruleStringCopy));
    // this->parse(input, g.rules, g.ws, peg::defaultErrorReporter, root);

    rete::ParserExceptionLocalized parserEx;
    auto reporter = [&rulestring, &parserEx](const peg::InputRange& err_in, const std::string& descr)
    {
        std::stringstream ss;

        // find row of error
        auto numNL = std::count(rulestring.begin(),
                                rulestring.begin() + err_in.begin().index(),
                                '\n');

        // find column of error
        auto rErrIt = rulestring.rbegin() + (rulestring.length() - err_in.begin().index());
        auto column = std::find(
                        rErrIt,
                        rulestring.rend(),
                        '\n');


        int indent = std::distance(rErrIt, column);

        ss << "Error parsing rules, at "
           << numNL << ":" << indent << '\n';

        for (size_t pos = 0; pos < err_in.begin().index(); pos++)
        {
            char c = rulestring[pos];
            ss << c;
        }
        ss << '\n';

        for (int i = 0; i+1 < indent; i++)
        {
            ss << "-";
        }
        ss << "^" << '\n';
        ss << descr << '\n';

//        std::cout << ss.str();

        parserEx = rete::ParserExceptionLocalized(ss.str(), numNL, indent, indent + (err_in.end().index() - err_in.begin().index()));
    };

    this->parse(input, g.rules, g.ws, reporter, root);

    if (!root) throw parserEx; // as prepared by the error reporter


    /*
        Preprocess: Substitute global constants and prefixes in the arguments
                    of conditions and effects
    */
    root->propagateDefinitionsToChildren();
    root->applyPrefixesAndGlobalConstantsDefinitionsToRules();

    /**
      Construct the network
    */
    std::vector<ParsedRule::Ptr> rules;

    // quick hack: fancy lambda to add recursive behaviour to parse all
    // nested rules, too
    std::function<void(std::unique_ptr<ast::Rules>&)>
    doParse = [this, &rules, &network, &doParse](std::unique_ptr<ast::Rules>& ast) -> void
    {
        for (auto& rule : ast->rules_)
        {
            auto pr = this->construct(*rule, network);
            rules.push_back(pr);
        }

        for (auto& nested : ast->scopedRules_)
        {
            doParse(nested);
        }
    };

    doParse(root);

    return rules;
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
#ifdef RETE_PARSER_VERBOSE
        std::cout << "Reusing AlphaNode " << (*it)->getDOTId() << std::endl;
#endif
        alpha = *it;
    }
    else
    {
#ifdef RETE_PARSER_VERBOSE
        std::cout << "Adding AlphaNode " << alpha->getDOTId() << " beneath " << parent->getDOTId()  << std::endl;
#endif
        SetParent(parent, alpha);
    }

    return alpha;
}


/**
    Try to find an equivalent node at the parent. If not found add the given node. Returns the node
    which is connected to the parent after this operation.
*/
BetaMemory::Ptr implementBetaNode(BetaNode::Ptr node, BetaMemory::Ptr parentBeta, AlphaMemory::Ptr parentAlpha)
{
    BetaNode::Ptr beta = node;
    BetaMemory::Ptr betamem = nullptr;

    std::vector<BetaNode::Ptr> candidates;
    parentBeta->getChildren(candidates);
    auto it = std::find_if(candidates.begin(), candidates.end(),
        [beta] (BetaNode::Ptr other) -> bool
        {
            return *beta == *other;
        }
    );

    // reuse or connect new, if the same join was found in the beta parent that connects to the wanted alpha
    if (it != candidates.end() && (*it)->getParentAlpha() == parentAlpha)
    {
#ifdef RETE_PARSER_VERBOSE
        std::cout << "Reusing BetaNode " << (*it)->getDOTId() << std::endl;
#endif
        beta = *it;
        betamem = beta->getBetaMemory();
    }
    else
    {
#ifdef RETE_PARSER_VERBOSE
        std::cout << "Adding BetaNode " << beta->getDOTId() << " beneath " << parentBeta->getDOTId() << " and " << (parentAlpha ? parentAlpha->getDOTId() : " 0x0") << std::endl;
#endif

        SetParents(parentBeta, parentAlpha, beta);
        // add a beta memory for the added node
        betamem.reset(new BetaMemory());
        SetParent(beta, betamem);
    }
    return betamem;
}

/**
    Find or insert the given BetaBetaNode.
*/
BetaMemory::Ptr implementBetaBetaNode(BetaBetaNode::Ptr node, BetaMemory::Ptr parentLeft, BetaMemory::Ptr parentRight)
{
    BetaBetaNode::Ptr betabeta = node;
    BetaMemory::Ptr betamem = nullptr;

    std::vector<BetaNode::Ptr> candidates;
    parentLeft->getChildren(candidates);
    auto it = std::find_if(candidates.begin(), candidates.end(),
        [betabeta](BetaNode::Ptr other) -> bool
        {
        return *betabeta == *other;
        }
    );

    if (it != candidates.end())
    {
        auto candidate = std::dynamic_pointer_cast<BetaBetaNode>(*it);
        if (candidate && candidate->getLeftParent() == parentLeft && candidate->getRightParent() == parentRight)
        {
#ifdef RETE_PARSER_VERBOSE
            std::cout << "Reusing BetaBetaNode " << candidate->getDOTId() << std::endl;
#endif
            // reuse node!
            betabeta = candidate;
            betamem = betabeta->getBetaMemory();
            return betamem;
        }
    }

#ifdef RETE_PARSER_VERBOSE
    std::cout << "Adding new BetaBetaNode " << betabeta->getDOTId() << " beneath " << parentLeft->getDOTId() << " and " << parentRight->getDOTId() << std::endl;
#endif
    // not found - insert new
    SetParents(parentLeft, parentRight, betabeta);
    betamem.reset(new BetaMemory());
    SetParent(betabeta, betamem);
    return betamem;
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
    else SetParents(nullptr, amem, adapter);

    return adapter;
}


/**
    Construct a primitive condition
*/
BetaMemory::Ptr RuleParser::constructPrimitive(
        Network& net,
        ast::Rule& rule,
        ast::Precondition& condition,
        BetaMemory::Ptr currentBeta,
        std::map<std::string, AccessorBase::Ptr>& bindings) const
{
    auto bIt = conditionBuilders_.find(condition.type());
    if (bIt == conditionBuilders_.end())
    {
        std::vector<std::string> knownTypes;
        for (auto& builder : conditionBuilders_) knownTypes.push_back(builder.first);
        throw NoBuilderException(rule.str_, condition.str_, condition.type(), knownTypes);
    }
    auto& builder = *bIt->second;
    // create an argument list. The ast::Arguments are consumed.
    // NOTE: When constructing an alpha node, variables are always unbound,
    // though they might have been used before in other alpha nodes of the same
    // rule! Bindings only happen in the beta network, when they are given
    // through the context of the sub-match. So, if we are constructing an
    // AlphaNode, create the arguments with an empty bindings-map.
    ArgumentList args;
    for (auto& astArg : condition.args_)
    {
        std::map<std::string, AccessorBase::Ptr> emptyBindings;
        std::map<std::string, AccessorBase::Ptr>* usedBindings = &emptyBindings;
        if (builder.builderType() != NodeBuilder::ALPHA)
            usedBindings = &bindings;

        Argument arg = Argument::createFromAST(std::move(astArg), *usedBindings);
        args.push_back(std::move(arg));
    }

    // Only relevant in case of an alpha node: Remember all the variables given
    // to the builder that were previously bound by another node. These are the
    // ones we need to create a join for in the beta network.
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
        try {
            builder.buildAlpha(args, anodes);
        } catch (NodeBuilderException& e) {
            // rethrow exception with added information
            e.setRule(rule.str_);
            e.setPart(condition.str_),
                throw;
        }

        AlphaNode::Ptr currentAlpha = net.getRoot();
        for (auto alpha : anodes)
        {
            currentAlpha = implementAlphaNode(alpha, currentAlpha);
        }

        auto amem = currentAlpha->getAlphaMemory();
        if (!amem)
        {
            amem.reset(new AlphaMemory());
            SetParent(currentAlpha, amem);
        }

        // - if there has been a beta node before, create a join
        if (currentBeta)
        {
            GenericJoin::Ptr join(new GenericJoin());

            // create one check for every variable that was previously unbound
            for (auto jv : joinVars)
            {
                Argument& arg = args[jv];
#ifdef RETE_PARSER_VERBOSE
                std::cout << "adding join check on variable " << arg.getVariableName() << std::endl;
#endif
                if (!arg.getAccessor())
                {
                    // the node did not bind the variable it was given? this is an error!
                    // but a from a development perspective. The nodebuilder was evil.
                    throw RuleConstructionException(rule.str_, condition.str_,
                            "The node builder left the variable " + arg.getVariableName() +
                            " unbound!");
                }

                AccessorBase::Ptr beta(bindings[arg.getVariableName()]->clone());
                AccessorBase::Ptr alpha(arg.getAccessor()->clone());

                // add the check to the join
                join->addCheck(beta, alpha);
            }

            // add the join.
            currentBeta = implementBetaNode(join, currentBeta, amem);
        }
        // - else create an AlphaBetaAdapter
        else
        {
            BetaNode::Ptr alphabeta = getAlphaBeta(currentAlpha->getAlphaMemory());
#ifdef RETE_PARSER_VERBOSE
            std::cout << "Adding AlphaBetaNode " << alphabeta << " beneath <nullptr> and " << currentAlpha << std::endl;
#endif

            BetaMemory::Ptr abmem = alphabeta->getBetaMemory();
            if (!abmem)
            {
                abmem.reset(new BetaMemory());
                SetParent(alphabeta, abmem);
            }
            currentBeta = abmem;
        } // endif (add join or ab-adapter)
    } // endif builderType() == ALPHA
    else if (builder.builderType() == NodeBuilder::BUILTIN)
    {
        // rules *must* start with an alpha check
        if (!currentBeta)
            throw RuleConstructionException(rule.str_, condition.str_,
                    "The first condition must create an alpha network, not a builtin!");

        // create and implement the builtin node
        try {
            Builtin::Ptr builtin = builder.buildBuiltin(args);
            currentBeta = implementBetaNode(builtin, currentBeta, nullptr);
        } catch (NodeBuilderException& e) {
            // rethrow with more information
            e.setRule(rule.str_);
            e.setPart(condition.str_);
            throw;
        }
    } // endif builderType() == BUILTIN


    // update the variable bindings
    // We want to override all bindings we can, so that we always have the
    // latest accessor for every variable (if one occurs in multiple conditions).
    for (auto& arg : args)
    {
        if (arg.isVariable())
        {
            auto accessor = arg.getAccessor();
            if (!accessor)
            {
                throw RuleConstructionException(
                        rule.str_, condition.str_,
                        "The node builder left the variable "
                        + arg.getVariableName() + " unbound!"); // a NodeBuilder left a variable unbound!
            }

            bindings[arg.getVariableName()] = accessor;
        }
    } // end for args update bindings

    return currentBeta;
} // end construct primitive


/**
    Construct a noValue group
*/
BetaMemory::Ptr RuleParser::constructNoValueGroup(
        Network& net,
        ast::Rule& rule,
        ast::NoValueGroup& noValueGroup,
        BetaMemory::Ptr currentBeta,
        std::map<std::string, AccessorBase::Ptr>& bindings) const
{
    if (currentBeta == nullptr)
    {
        throw RuleConstructionException(rule.str_, noValueGroup.str_, "\"noValue { ... }\" cannot be the first condition in a rule");
    }

    /*
        To implement a noValueGroup we have to:
        1. Add all its conditions as usual
        2. Connect the start and the end of the noValueGroup with a noValue-node
        3. Treat the conditions as if they never happened, because there will be literally "noValue".
           That means that we have to reset the variable bindings and only account
           for the one empty wme that the NoValue node adds.
    */

    // 0. make a copy of the bindings to work with while constructing the group
    std::map<std::string, AccessorBase::Ptr> tmpBindings;
    for (auto entry : bindings)
    {
        tmpBindings[entry.first] = AccessorBase::Ptr(entry.second->clone());
    }
    BetaMemory::Ptr newBeta = currentBeta;

    // 1. add all the conditions as usual
    for (auto& condition : noValueGroup.conditions_)
    {
        newBeta = constructCondition(rule, net, newBeta, tmpBindings, *condition);
    }

    // 2. add the noValue node
    //    the size difference between the tokens that will arrive at both points
    //    is exactly the number of conditions in the group:
    //    - every primitive will lead to a single join
    //    - every noValueGroup will only add an EmptyWME
    auto noValueNode = std::make_shared<NoValue>(noValueGroup.conditions_.size());
    return implementBetaBetaNode(noValueNode, currentBeta, newBeta);
}



/*
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


/**
    Implement the rule in the given network.
*/
ParsedRule::Ptr RuleParser::construct(ast::Rule& rule, Network& net) const
{
    ParsedRule::Ptr ruleInfo = ParsedRule::Ptr(new ParsedRule());
    ruleInfo->ruleString_ = rule.str_;

    // keep track of the last implemented beta memory
    BetaMemory::Ptr currentBeta = nullptr;

    // remember already bound variables
    std::map<std::string, AccessorBase::Ptr> bindings;

    // construct all the conditions
    for (auto& condition : rule.conditions_)
    {
      currentBeta = constructCondition(rule, net, currentBeta, bindings, *condition);
    } // end loop over conditions


    //  create productions / effects
    size_t effectNo = 0;
    for (auto& effect : rule.effects_)
    {
        // find the correct builder
        auto it = effectBuilders_.find(effect->type());
        if (it == effectBuilders_.end())
        {
            std::vector<std::string> knownTypes;
            for (auto& builder : effectBuilders_) knownTypes.push_back(builder.first);
            throw NoBuilderException(rule.str_, effect->str_, effect->type(), knownTypes);
        }

        // create an argument list. The ast::Arguments are consumed.
        ArgumentList args;
        for (auto& astArg : effect->args_)
        {
            Argument arg = Argument::createFromAST(std::move(astArg), bindings);
            // all args in the effects MUST be bound variables or constants!
            if (arg.isVariable() && !arg.getAccessor())
            {
                throw RuleConstructionException(rule.str_, effect->str_,
                        "Unbound variable in effect: " + arg.getVariableName()); // unbound variable in rule effect
            }
            args.push_back(std::move(arg));
        }

        // create the production, encapsulate it in an AgendaNode, and add it to the current pattern results (beta memory)
        try {
            auto production = it->second->buildEffect(args);
            AgendaNode::Ptr agendaNode(new AgendaNode(production, net.getAgenda()));

            if (rule.name_)
            {
                ruleInfo->name_ = *rule.name_;
                agendaNode->setName(*rule.name_);
                production->setName(*rule.name_ + "[" + std::to_string(effectNo) + "]" );
            }
            else
            {
                agendaNode->setName("");
                // use the verbose description of the effect if the rule has no name
                production->setName(production->toString());
            }
            SetParent(currentBeta, agendaNode);
            // net.addProduction(agendaNode);
            ruleInfo->effectNodes_.push_back(agendaNode);

        } catch (NodeBuilderException& e) {
            // rethrow with more information
            e.setRule(rule.str_);
            e.setPart(effect->str_);
            throw;
        }
        effectNo++;
    }

    return ruleInfo;
}


BetaMemory::Ptr RuleParser::constructCondition(
        ast::Rule& rule,
        Network& net,
        BetaMemory::Ptr currentBeta,
        std::map<std::string, AccessorBase::Ptr>& bindings,
        ast::PreconditionBase& condition) const
{
    if (condition.isPrimitive())
    {
        auto& primitive = dynamic_cast<ast::Precondition&>(condition);
        currentBeta = constructPrimitive(net, rule, primitive, currentBeta, bindings);
    }
    else if (condition.isNoValueGroup())
    {
        auto& nvGroup = dynamic_cast<ast::NoValueGroup&>(condition);
        currentBeta = constructNoValueGroup(net, rule, nvGroup, currentBeta, bindings); // recursive call
    }
    else if (condition.isGroupBy())
    {
        auto& groupBy = dynamic_cast<ast::GroupBy&>(condition);
        currentBeta = constructGroupBy(rule, groupBy, currentBeta, bindings);
    }
    else
    {
        throw RuleConstructionException(rule.str_, condition.str_, "Condition is neither primitive nor a noValueGroup. What have you done?!");
    }

    /*
       Every time we created a beta node (join, builtin, alphabeta, noValue), we need to increment
       the accessors afterwards. ASSUMPTION: The accessors bound to variables by the node
       builders are indexed at -1, so that we can just increment them all and get them to
       index 0, the first wme in the token chain.
    */
    for (auto binding : bindings)
    {
        ++(binding.second->index());
    }

#ifdef RETE_PARSER_VERBOSE
    std::cout << "currentBeta: " << (currentBeta ? currentBeta->getDOTId() : "nullptr") << std::endl;
#endif

    return currentBeta;
}


BetaMemory::Ptr RuleParser::constructGroupBy(
        ast::Rule& rule,
        ast::GroupBy& groupBy,
        BetaMemory::Ptr currentBeta,
        std::map<std::string, AccessorBase::Ptr>& bindings) const
{
    /*
        Construct a GroupBy-Node,
        give it the accessors to the grouping-variables,
        update the accessors:
            - grouping vars -> special wrapper that just forwards the existing
              accessor to the first entry in the group (all entries in the group
              have the same value for the grouping variable)
            - all other vars -> Wrap in TokenGroupAccessor
    */
    auto node = std::make_shared<GroupBy>();
    for (auto& var : groupBy.variables_)
    {
        auto acc = bindings[*var];
        if (!acc)
            throw RuleConstructionException(rule.str_, groupBy.str_,
                    "cannot group on unbound variable " + *var);

        std::unique_ptr<AccessorBase> accClone(acc->clone());
        node->addCriteria(std::move(accClone));
    }

    currentBeta = implementBetaNode(node, currentBeta, nullptr);

    for (auto entry : bindings)
    {
        std::unique_ptr<AccessorBase> clone(entry.second->clone());

        if (std::find_if(groupBy.variables_.begin(),
                      groupBy.variables_.end(),
                      [name = entry.first](auto& var) -> bool
                      {
                        return *var == name;
                      }) != groupBy.variables_.end())
        {
            // its a variable to match on -> forward accessor
            bindings[entry.first] =
                std::make_shared<TokenGroupAccessorForwarder>(std::move(clone));
        }
        else
        {
            // different value for this var in the group -- TokenGroupAccessor
            bindings[entry.first] =
                std::make_shared<TokenGroupAccessor>(std::move(clone));
        }
    }

    return currentBeta;
}


std::vector<std::string> RuleParser::listAvailableConditions() const
{
    std::vector<std::string> result;
    for (auto& entry : conditionBuilders_)
    {
        result.push_back(entry.first);
    }
    return result;
}

std::vector<std::string> RuleParser::listAvailableEffects() const
{
    std::vector<std::string> result;
    for (auto& entry : effectBuilders_)
    {
        result.push_back(entry.first);
    }
    return result;
}

} /* rete */
