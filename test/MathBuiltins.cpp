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
R"delimiter(

[computation:
    (?a <foo> ?b), sum(?sum 1 1.0 100.0e-2), mul(?neg ?sum -1), div(?quot ?neg -3.0)
        -> (<a> <sum> ?sum), (<a> <neg> ?neg), (<a> <quot> ?quot)]
[debug: (<a> ?b ?c) -> (<just> <lookat> <alphamemory>)]
[circleArea: (?circle <radius> ?r), mul(?area 3.14159265 ?r ?r) -> (?circle <area> ?area)]
[debug2: (?a <area> ?b) -> (<just> <lookat> <alphamemory>)]

)delimiter",
        reasoner.net()
    );

    if (!ok) return 1;


    int asserted = 0;
    int retracted = 0;
    auto callback = [&](WME::Ptr wme, PropagationFlag flag)
    {
        std::cout << (flag == PropagationFlag::ASSERT ? "asserted: " : "retracted: ") << wme->toString() << std::endl;
        if (flag == PropagationFlag::ASSERT) asserted++;
        if (flag == PropagationFlag::RETRACT) retracted++;
    };
    reasoner.setCallback(callback);


    Triple::Ptr wme0(new Triple("<circle1>","<radius>", "1.415"));
    Triple::Ptr wme1(new Triple("<1>","<foo>", "<2>"));
    AssertedEvidence::Ptr evidence0(new AssertedEvidence("Evidence0"));
    AssertedEvidence::Ptr evidence1(new AssertedEvidence("Evidence1"));
    reasoner.addEvidence(wme0, evidence0);
    reasoner.addEvidence(wme1, evidence1);
    reasoner.performInference();

    save(reasoner.net(), "mathbuiltins.dot");

    // expect only assertions for ?sum, ?neg, ?quot, ?area and the dummy (just lookat alphamemory).
    // --> 5 assertions. Plus the two asserted evidences! --> 7
    if (asserted != 7) return 2;
    if (retracted != 0) return 3;

    reasoner.removeEvidence(evidence1);
    reasoner.performInference();
    // expect retraction of evidence1, ?sum, ?neg ?quot, but dummy and area still hold
    save(reasoner.net(), "mathbuiltins.dot");

    if (asserted != 7) return 4;
    if (retracted != 4) return 5;

    return 0;
}
