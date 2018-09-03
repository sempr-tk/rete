#include "../include/RuleParser.hpp"
#include "../parserlib/source/parserlib.hpp"
#include "../include/RuleParserAST.hpp"

#include <iostream>

namespace rete {
namespace pl = parserlib;

bool RuleParser::parseRules(std::string rulestring)
{
    // special character groups
    pl::rule endl = pl::nl(pl::expr("\r\n") | "\n\r" | '\n' | '\r');
    pl::rule ws = *(pl::set(" \t") | endl);
    pl::rule alpha = (pl::range('a', 'z') | pl::range('A', 'Z'));
    pl::rule num = pl::range('0', '9');
    pl::rule alphanum = alpha | num;
    // // escape-chars, e.g. \t, \n, \\, \", ...
    pl::rule echar = pl::expr('\\') >> (pl::set("tbnrf\"\'\\"));

    // iri: between <> are some unallowed chars
    pl::rule iriref =  pl::expr('<') >>
                            *(!(pl::set("<>\"{}|^`\\") | pl::range('\x00', '\x20')) >> pl::any()) >>
                        pl::expr('>')
                      ;


    // string literal in quotes: some unallowed things, but escape-chars are okay
    pl::rule stringliteralquote =
            pl::expr('\"') >>
                *( (!pl::set("\x22\x5C\xA\xD") >> pl::any())
                    | echar ) >>
            pl::expr('\"');

    pl::rule langtag = pl::expr('@') >> +alpha >> *(pl::expr('-') >> +alphanum);
    pl::rule literal = stringliteralquote >> -(pl::expr("^^") >> iriref | langtag);

    pl::rule blank_node_label = pl::expr("_:") >> *alphanum;

    pl::rule subject =      iriref | blank_node_label;
    pl::rule predicate =    iriref;
    pl::rule object =       iriref | blank_node_label | literal;

    pl::rule triple = pl::expr('(')
                        >> subject
                        >> predicate
                        >> object
                        >> pl::expr(')');

    pl::rule triples = triple >> *(pl::expr(',') >> triple);
    pl::rule rule = pl::expr('[') >>
                        triples >> pl::expr('-') >> pl::expr('>') >> triples >>
                    pl::expr(']');

    pl::rule rules = +rule;

    pl::ast<ast::String> ast_subject(subject);
    pl::ast<ast::String> ast_predicate(predicate);
    pl::ast<ast::String> ast_object(object);

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
        std::cout << "left: " << std::endl;
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
    }

    return false;
}

} /* rete */
