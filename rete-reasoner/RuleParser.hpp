#ifndef RETE_RULE_PARSER_HPP_
#define RETE_RULE_PARSER_HPP_

#include <string>
#include <rete-core/Network.hpp>

#include "RuleGrammar.hpp"
#include "RuleParserAST.hpp"
#include "NodeBuilder.hpp"

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

    peg::BindAST<peg::ASTString> prefixname = RuleGrammar::get().prefixname;
    peg::BindAST<ast::PrefixDefinition> prefix = RuleGrammar::get().prefixdef;

    // peg::BindAST<ast::Argument> subject = RuleGrammar::get().subject;
    // peg::BindAST<ast::Argument> predicate = RuleGrammar::get().predicate;
    // peg::BindAST<ast::Argument> object = RuleGrammar::get().object;

    peg::BindAST<ast::Argument> argument = RuleGrammar::get().argument;
    peg::BindAST<ast::Variable> variable = RuleGrammar::get().variable;
    peg::BindAST<ast::Number> number = RuleGrammar::get().number;
    peg::BindAST<ast::QuotedString> quotedstring = RuleGrammar::get().quotedString;
    peg::BindAST<ast::URI> uri = RuleGrammar::get().uri;

    // note: Adding base class of preconditions leads to duplicates / empty objects. AST objects are constructed in reverse order than they are parsed, so while it's tried to parse "precondition" by parsing "triple", if "triple" matches and thus "precondition" matches, too, the triple is parsed first (and with it a precondition, since it subclasses one), and afterwards the precondition-match is tried to construct into a AST node, leading to an empty precondition.
    // peg::BindAST<ast::Precondition> precondition = RuleGrammar::get().precondition;
    // preconditions
    peg::BindAST<ast::Triple> triple = RuleGrammar::get().triple;
    peg::BindAST<peg::ASTString> builtinname = RuleGrammar::get().builtinName;
    peg::BindAST<ast::Builtin> builtin = RuleGrammar::get().builtin;

    // effects
    peg::BindAST<ast::InferTriple> inferTriple = RuleGrammar::get().inferTriple;

    // rules
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

    void construct(ast::Rule&, Network&) const;


public:
    RuleParser();

    /**
        Register a new NodeBuilder to extent the capabilities of the parser.

        Throws if a builder for the same node identifier is already registered.
    */
    void registerNodeBuilder(std::unique_ptr<NodeBuilder> builder);
    template <class T, typename = typename std::enable_if<std::is_base_of<NodeBuilder, T>::value, void>::type>
    void registerNodeBuilder()
    {
        registerNodeBuilder(std::unique_ptr<T>(new T()));
    }


    /**
        Interprets the given string as a definition of rules and constructs them in the given
        network.
        \arg rules the string defining the rules
        \arg network the rete network to integrate the rules into
        \return true if parsing and construction of the rules was successfull
        TODO: throw an exception with more information when the rule syntax was violated and the parsing failed.
    */
    bool parseRules(const std::string& rules, Network& network);

    const RuleGrammar& g = RuleGrammar::get();
};

} /* rete */

#endif /* end of include guard: RETE_RULE_PARSER_HPP_ */
