#include <iostream>
#include <fstream>

#include <rete-core/ReteCore.hpp>
#include <rete-rdf/ReteRDF.hpp>

#include <rete-reasoner/Reasoner.hpp>
#include <rete-reasoner/AssertedEvidence.hpp>

#include <rete-reasoner/RuleParser.hpp>

using namespace rete;


void save(Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}

int main()
{
    RuleParser p;
    Reasoner reasoner;

    p.parseRules(
        R"foo(
            [(?a <foo> <bar>), (?b <foo> <baz>), (?a ?b ?c) -> (<foo> <bar> <baz>)]
        )foo",
        reasoner.net());

//    /**
//        Bad rule design! The first two conditions in the second rule are swapped, so a different
//        joint node will be created, the sub-network duplicated.
//    */
//    p.parseRules(
//        "[(?a <foo> ?b), (?b <bar> ?c), (?c <baz> ?d) -> (?a <foobaz> ?d)]\n"
//        "[somerandomname42: (?b <bar> ?c), (?a <foo> ?b), (?c <zab> ?d) -> (?a <foozab> ?d)]\n",
//        reasoner.net()
//    );
//
    save(reasoner.net(), "badRules.dot");
//
//    // -----
//
//    RuleParser p2;
//    Reasoner reasoner2;
//
//    /**
//        Good rule design: The first two conditions match in the two rules, so this subnetwork is
//        reused.
//    */
//    p2.parseRules(
//        "[name1:(?a <foo> ?b), (?b <bar> ?c), (?c <baz> ?d) -> (?a <foobaz> ?d)]\n"
//        "[name2:(?a <foo> ?b), (?b <bar> ?c), (?c <zab> ?d) -> (?a <foozab> ?d)]",
//        reasoner2.net()
//    );
//
//    save(reasoner2.net(), "goodRules.dot");

    return 0;
}
