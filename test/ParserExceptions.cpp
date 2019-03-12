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

    // don't throw on a simple syntax error for now
    bool ok = p.parseRules(
        "[rule1: (?a <foo ?b) -> (?b <foo> ?a)]\n",
        reasoner.net()
    );
    if (ok) return 1; // shouldn't be "ok"

    // unknown alpha condition
    try {
        p.parseRules(
            "[rule2: foo(?a) -> (?b <foo> ?a)]\n",
            reasoner.net()
            );
        return 2; // no exception? --> error!
    } catch (ParserException& e) {
        std::cout << e.what() << std::endl;
    }

    // noValue at first condition
    try {
        p.parseRules(
            "[rule3: noValue (?a <foo> ?b) -> (?b <foo> ?a)]\n",
            reasoner.net()
            );
        return 3; // no exception? --> error!
    } catch (ParserException& e) {
        std::cout << e.what() << std::endl;
    }

    // unbound variable in effect
    try {
        p.parseRules(
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
        p.parseRules(
            "[rule5: (?a <foo> ?b), sum(?sum ?b ?c) -> (?b <foo> ?a)]\n",
            reasoner.net()
            );
        return 5; // no exception? --> error!
    } catch (ParserException& e) {
        std::cout << e.what() << std::endl;
    }

    // not a number constant in math builtin
    try {
        p.parseRules(
            "[rule6: (?a <foo> ?b), sum(?sum 1 \"abc\") -> (?b <foo> ?a)]\n",
            reasoner.net()
            );
        return 5; // no exception? --> error!
    } catch (ParserException& e) {
        std::cout << e.what() << std::endl;
    }

    // net network must be really ugly by now :D
    save(reasoner.net(), "prefixes.dot");

    return 0;
}
