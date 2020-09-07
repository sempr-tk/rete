#include <iostream>
#include <fstream>

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-reasoner/Exceptions.hpp"

using namespace rete;


void save(Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}


/**
    Provoke exceptions through invalid rule definitions, that either go against the defined syntax
    or lead to problems in the construction phase.
*/
int main()
{
    RuleParser p;
    Reasoner reasoner;

    std::vector<rete::ParsedRule::Ptr> rules;

    try {
        rules = p.parseRules(
            "[rule1: (?a <foo ?b) -> (?b <foo> ?a)]\n",
            reasoner.net()
        );
        return 1; // no exception? --> error!
    } catch (ParserException& e) {
        std::cout << e.what() << std::endl;
    }

    // unknown alpha condition
    try {
        rules = p.parseRules(
            "[rule2: foo(?a) -> (?b <foo> ?a)]\n",
            reasoner.net()
        );
        return 2; // no exception? --> error!
    } catch (ParserException& e) {
        std::cout << e.what() << std::endl;
    }

    // noValue at first condition
    try {
        rules = p.parseRules(
            "[rule3: noValue { (?a <foo> ?b) } -> (?b <foo> ?a)]\n",
            reasoner.net()
        );
        return 3; // no exception? --> error!
    } catch (ParserException& e) {
        std::cout << e.what() << std::endl;
    }

    // unbound variable in effect
    try {
        rules = p.parseRules(
            "[rule4: (?a <foo> ?b) -> (?b <foo> ?d)]\n",
            reasoner.net()
        );
        return 4; // no exception? --> error!
    } catch (ParserException& e) {
        std::cout << e.what() << std::endl;
    }

    // builtin leaves variable unbound
    // (actually, the nodebuilder checks this itself and throws an exception)
    try {
        rules = p.parseRules(
            "[rule5: (?a <foo> ?b), sum(?sum ?b ?c) -> (?b <foo> ?a)]\n",
            reasoner.net()
        );
        return 5; // no exception? --> error!
    } catch (ParserException& e) {
        std::cout << e.what() << std::endl;
    }

    // not a number constant in math builtin
    try {
        rules = p.parseRules(
            "[rule6: (?a <foo> ?b), sum(?sum 1 \"abc\") -> (?b <foo> ?a)]\n",
            reasoner.net()
        );
        return 6; // no exception? --> error!
    } catch (ParserException& e) {
        std::cout << e.what() << std::endl;
    }

    // unknown production
    try {
        rules = p.parseRules(
            "[rule7: (?a <foo> ?b) -> whatever(?b)]",
            reasoner.net()
        );
        return 7;
    } catch (ParserException& e) {
        std::cout << e.what() << std::endl;
    }

    // syntax error in multiline rule
    try {
        rules = p.parseRules(
            "[rule8:\n"
            "    (?a ?b ?c),\n"
            "    (?c ?d ?)\n"
            " ->\n"
            "    (?a ?e ?c)]",
            reasoner.net()
        );
    save(reasoner.net(), "exceptions.dot");
        return 8;
    } catch (ParserException& e) {
        std::cout << e.what() << std::endl;
    }

    // net network must be really ugly by now :D
    save(reasoner.net(), "exceptions.dot");

    return 0;
}
