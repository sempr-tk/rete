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
        "[rule1: true() -> (<a> <b> <c>)]\n",
        reasoner.net()
    );

    reasoner.performInference();
    auto wmes = reasoner.getCurrentState().getWMEs();

    // there should be exactly one WME, the (<a> <b> <c>) triple
    if (wmes.size() != 1) return 2;
    auto wme = std::dynamic_pointer_cast<Triple>(wmes[0]);
    if (!wme) return 3;
    if (!(*wme == Triple("<a>", "<b>", "<c>"))) return 4;


    // add another rule depending on true and (<a> <b> <c>)
    auto moreRules = p.parseRules(
        "[rule2: (<a> <b> <c>), true() -> (<d> <e> <f>)]",
        reasoner.net()
    );

    reasoner.performInference();
    wmes = reasoner.getCurrentState().getWMEs();

    if (wmes.size() != 2) return 5;

    save(reasoner.net(), "true_test.dot");

    return 0;
}
