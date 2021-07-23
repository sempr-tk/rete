#ifndef RETE_RULE_PARSER_HPP_
#define RETE_RULE_PARSER_HPP_

#include <string>
#include "../rete-core/Network.hpp"

#include "RuleGrammar.hpp"
#include "RuleParserAST.hpp"
#include "NodeBuilder.hpp"
#include "ParsedRule.hpp"

#define USE_RTTI
#include <pegmatite/pegmatite.hh>

#include <map>
#include <type_traits>

namespace rete {

    namespace peg = pegmatite;

/**
    Parses rules that are given in text format, e.g.
    [(?a rdf:subClassOf ?b), (?b rdf:subClassOf ?c) -> (?a rdf:subClassOf ?c)]
*/
class RuleParser : peg::ASTParserDelegate {
    peg::BindAST<ast::URI> iriref     = RuleGrammar::get().iriref;
    peg::BindAST<ast::Argument> literal     = RuleGrammar::get().literal; // TODO subclass Argument!
    peg::BindAST<ast::URI> prefixedURI     = RuleGrammar::get().prefixedURI;
    peg::BindAST<ast::URI> blank_node_label     = RuleGrammar::get().blank_node_label;
    peg::BindAST<ast::GlobalConstantReference> globalConstRef = RuleGrammar::get().globalConstID;
    peg::BindAST<ast::GlobalConstantDefinition> globalConstDef = RuleGrammar::get().globalConstDef;

    peg::BindAST<peg::ASTString> overrideFlag = RuleGrammar::get().overrideFlag;
    peg::BindAST<peg::ASTString> prefixname = RuleGrammar::get().prefixname;
    peg::BindAST<ast::PrefixDefinition> prefix = RuleGrammar::get().prefixdef;

    // peg::BindAST<ast::Argument> subject = RuleGrammar::get().subject;
    // peg::BindAST<ast::Argument> predicate = RuleGrammar::get().predicate;
    // peg::BindAST<ast::Argument> object = RuleGrammar::get().object;

    // peg::BindAST<ast::Argument> argument = RuleGrammar::get().argument;
    peg::BindAST<ast::Variable> variable = RuleGrammar::get().variable;
    //peg::BindAST<ast::Number> number = RuleGrammar::get().number;
    peg::BindAST<ast::Float> floating = RuleGrammar::get().floating;
    peg::BindAST<ast::Int> integer = RuleGrammar::get().integer;

    peg::BindAST<ast::QuotedString> quotedstring = RuleGrammar::get().quotedString;
    // peg::BindAST<ast::URI> uri = RuleGrammar::get().uri;

    // note: Adding base class of preconditions leads to duplicates / empty objects. AST objects are constructed in reverse order than they are parsed, so while it's tried to parse "precondition" by parsing "triple", if "triple" matches and thus "precondition" matches, too, the triple is parsed first (and with it a precondition, since it subclasses one), and afterwards the precondition-match is tried to construct into a AST node, leading to an empty precondition.
    // peg::BindAST<ast::Precondition> precondition = RuleGrammar::get().precondition;
    // preconditions
    peg::BindAST<ast::Triple> triple = RuleGrammar::get().triple;
    peg::BindAST<peg::ASTString> builtinname = RuleGrammar::get().builtinName;
    peg::BindAST<ast::Builtin> builtin = RuleGrammar::get().builtin;

    peg::BindAST<peg::ASTString> alphaConName = RuleGrammar::get().alphaConditionName;
    peg::BindAST<ast::GenericAlphaCondition> alphacon = RuleGrammar::get().genericAlphaCondition;

    peg::BindAST<ast::NoValueGroup> noValueGroup = RuleGrammar::get().noValueGroup;
    peg::BindAST<ast::GroupBy> groupBy = RuleGrammar::get().groupBy;

    peg::BindAST<peg::ASTString> effname = RuleGrammar::get().effectName;
    peg::BindAST<ast::GenericEffect> geneffect = RuleGrammar::get().genericEffect;
    // effects
    peg::BindAST<ast::InferTriple> inferTriple = RuleGrammar::get().inferTriple;

    peg::BindAST<ast::EffectIfBranch> effectListIf = RuleGrammar::get().ifBranch;
    peg::BindAST<peg::ASTString> elseMarker = RuleGrammar::get().elseMarker;
    peg::BindAST<ast::EffectElseBrach> effectListElse = RuleGrammar::get().elseBranch;

    peg::BindAST<ast::AnnotatedEffects> annotatedEffects = RuleGrammar::get().annotatedEffects;
    peg::BindAST<ast::UnannotatedEffects> unannotatedEffects = RuleGrammar::get().unannotatedEffects;

    // annotations
    peg::BindAST<peg::ASTString> varNameInRefInAnnotation = RuleGrammar::get().varNameInRefInAnnotation;
    peg::BindAST<ast::Annotation> annotationText = RuleGrammar::get().annotationText;
    peg::BindAST<ast::AnnotatedConditions> annotatedConditions = RuleGrammar::get().annotatedConditions;
    peg::BindAST<ast::UnannotatedConditions> unannotatedConditions = RuleGrammar::get().unAnnotatedConditions;

    // rules
    peg::BindAST<peg::ASTString> rulename = RuleGrammar::get().rulename;
    peg::BindAST<ast::Rule> rule = RuleGrammar::get().rule;
    peg::BindAST<ast::Rules> rules = RuleGrammar::get().rules;


    /*
        To allow extension of the supported WMEs (--> alpha network nodes) and builtins
        (beta-network nodes), the parser keeps a list of known condition types and maps them to
        NodeBuilder objects that are called to constuct the nodes we need. The connection is then
        again done by the parser, which also searches for identical nodes that might be reused.
    */
    std::map<std::string, std::unique_ptr<NodeBuilder>> conditionBuilders_;
    std::map<std::string, std::unique_ptr<NodeBuilder>> effectBuilders_;

    /**
        Constructs a rule from its ast representation in the network, and
        returns a struct containing information about the rule, including the
        effect nodes that hold the corresponding part of the network alive.
    */
    ParsedRule::Ptr construct(ast::Rule&, Network&) const;

    /**
        Helper function doing the actual work, designed for recursive parsing
        of e.g. noValue groups, returns the new current beta node, updates the
        bindings.
    */
    BetaMemory::Ptr constructCondition(
            ast::Rule& rule,
            Network& net,
            BetaMemory::Ptr currentBeta,
            std::map<std::string, AccessorBase::Ptr>& bindings,
            ast::PreconditionBase& condition) const;

    ProductionNode::Ptr constructEffect(
            ast::Rule& rule,
            Network& net,
            ast::Effect& effect,
            const std::string& name,
            BetaMemory::Ptr betaMem,
            const std::map<std::string, AccessorBase::Ptr>& bindings) const;


    std::vector<rete::ProductionNode::Ptr> constructSubRule(
            ast::Rule& subRule,
            Network& net,
            std::map<std::string, AccessorBase::Ptr>& bindings,
            std::vector<Annotation> conditionAnnotations,
            BetaMemory::Ptr currentBeta,
            const std::string& namePrefix = "") const;


    BetaMemory::Ptr constructPrimitive(
            Network&,
            ast::Rule&,
            ast::Precondition&,
            BetaMemory::Ptr,
            std::map<std::string, AccessorBase::Ptr>&) const;

    BetaMemory::Ptr constructNoValueGroup(
            Network&,
            ast::Rule&,
            ast::NoValueGroup&,
            BetaMemory::Ptr,
            std::map<std::string, AccessorBase::Ptr>&) const;

    BetaMemory::Ptr constructGroupBy(
            ast::Rule&,
            ast::GroupBy&,
            BetaMemory::Ptr,
            std::map<std::string, AccessorBase::Ptr>&) const;

public:
    RuleParser();

    /**
        Register a new NodeBuilder to extent the capabilities of the parser.

        Throws if a builder for the same node identifier is already registered.
    */
    void registerNodeBuilder(std::unique_ptr<NodeBuilder> builder);
    template <class T, class... Args>
    void registerNodeBuilder(Args&&... args)
    {
        registerNodeBuilder(std::unique_ptr<T>(new T(std::forward<Args>(args)...)));
    }


    /**
        Interprets the given string as a definition of rules and constructs them in the given
        network.
        \arg rules the string defining the rules
        \arg network the rete network to integrate the rules into
        \return the list of parsed rules. If these run out of scope the network
                will be deconstructed automatically!
    */
    std::vector<ParsedRule::Ptr> parseRules(
            const std::string& rules,
            Network& network) __attribute__((warn_unused_result));

    const RuleGrammar& g = RuleGrammar::get();

    /**
        Get a list of available conditions --
        the names registered to NodeBuilders.
    */
    std::vector<std::string> listAvailableConditions() const;

    /**
        Get a list of available effects --
        the names registered to NodeBuilders.
    */
    std::vector<std::string> listAvailableEffects() const;

};

} /* rete */

#endif /* end of include guard: RETE_RULE_PARSER_HPP_ */
