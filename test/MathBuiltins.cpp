#include <iostream>
#include <fstream>

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-reasoner/AssertedEvidence.hpp"
#include "../rete-rdf/Triple.hpp"

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

    // TODO: InferTriple only accepts string so for. Need to implement a more versatile ValueAccessor.
    bool ok = p.parseRules(
"[(?a <foo> ?b), sum(?sum 1 1.0 100.0e-2), mul(?neg ?sum -1), div(?quot ?neg -3.0) -> (<a> <b> <c>)]\n",
        reasoner.net()
    );

    Triple::Ptr wme(new Triple("<1>","<foo>", "<2>"));
    AssertedEvidence::Ptr evidence(new AssertedEvidence(""));
    reasoner.addEvidence(wme, evidence);
    reasoner.performInference();

    save(reasoner.net(), "mathbuiltins.dot");

    return (ok ? 0 : 1);
}
