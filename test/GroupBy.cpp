#include <iostream>
#include <fstream>

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-rdf/ReteRDF.hpp"

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

    /*
        Check if the true() condition works as expeted, i.e. always holds,
        and triggers rules exactly once.
    */
    auto rules = p.parseRules(
        "[dataRule: true()"
        "        -> (<a1> <foo> <b1>), (<a1> <foo> <b2>), (<a1> <foo> <b3>),"
        "           (<a2> <foo> <b1>), (<a2> <foo> <b2>)]\n"
        "[testRule: (?a <foo> ?b), GROUP BY (?a) -> (<a group> <for> ?a)]",
        reasoner.net()
    );

    reasoner.performInference();

    save(reasoner.net(), "group_test.dot");

    auto wmes = reasoner.getCurrentState().getWMEs();

    // there should be exactly 5+2 WMEs (5 from the data rule, put into 2 groups)
    if (wmes.size() != 7) return 1;

    return 0;
}
