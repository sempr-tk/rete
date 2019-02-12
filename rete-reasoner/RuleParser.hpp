#ifndef RETE_RULE_PARSER_HPP_
#define RETE_RULE_PARSER_HPP_

#include <string>
#include <rete-core/Network.hpp>

#include "RuleGrammar.hpp"
#include "RuleParserAST.hpp"

#define USE_RTTI
#include <pegmatite/pegmatite.hh>

namespace rete {

    namespace peg = pegmatite;

/**
    Parses rules that are given in text format, e.g.
    [(?a rdf:subClassOf ?b), (?b rdf:subClassOf ?c) -> (?a rdf:subClassOf ?c)]
*/
class RuleParser : peg::ASTParserDelegate {
    peg::BindAST<peg::ASTString> iriref     = RuleGrammar::get().iriref;
    peg::BindAST<peg::ASTString> variable     = RuleGrammar::get().variable;
    peg::BindAST<peg::ASTString> literal     = RuleGrammar::get().literal;
    peg::BindAST<peg::ASTString> prefixedURI     = RuleGrammar::get().prefixedURI;
    peg::BindAST<peg::ASTString> blank_node_label     = RuleGrammar::get().blank_node_label;

    peg::BindAST<peg::ASTString> prefixname = RuleGrammar::get().prefixname;
    peg::BindAST<ast::PrefixDefinition> prefix = RuleGrammar::get().prefixdef;

    peg::BindAST<ast::TripleElement> subject = RuleGrammar::get().subject;
    peg::BindAST<ast::TripleElement> predicate = RuleGrammar::get().predicate;
    peg::BindAST<ast::TripleElement> object = RuleGrammar::get().object;

    peg::BindAST<ast::Triple> triple = RuleGrammar::get().triple;
    peg::BindAST<ast::Triples> triples = RuleGrammar::get().triples;
    peg::BindAST<ast::Rule> rule = RuleGrammar::get().rule;
    peg::BindAST<ast::Rules> rules = RuleGrammar::get().rules;



public:
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
