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

    /**
    */
    bool ok = p.parseRules(
        "[rule1: (?a <type> <foo>), noValue (?a <color> <red>) -> (?a <not-known-to-be> <red>)]",
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


    Triple::Ptr wme0(new Triple("<a>","<type>", "<foo>"));
    Triple::Ptr wme1(new Triple("<b>","<type>", "<foo>"));
    Triple::Ptr wme2(new Triple("<c>","<type>", "<foo>"));
    Triple::Ptr wme3(new Triple("<a>", "<color>", "<red>"));
    AssertedEvidence::Ptr evidence0(new AssertedEvidence("Evidence0"));
    AssertedEvidence::Ptr evidence1(new AssertedEvidence("Evidence1"));

    reasoner.addEvidence(wme0, evidence0);
    reasoner.addEvidence(wme1, evidence0);
    reasoner.addEvidence(wme2, evidence0);
    reasoner.performInference();
    save(reasoner.net(), "novalue_0_init.dot");

    // 3 assertions, +1 inferred each = 6 assertions
    if (asserted != 6 || retracted != 0) return 2;

    reasoner.addEvidence(wme3, evidence1);
    reasoner.performInference();
    save(reasoner.net(), "novalue_1_onecolour.dot");

    // 1 more assertion, but also 1 retraction due to the negative join on red color
    if (asserted != 7 || retracted != 1) return 3;

    reasoner.removeEvidence(evidence1);
    reasoner.performInference();
    save(reasoner.net(), "novalue_2_nomorecolour.dot");

    // 1 more retraction, but also 1 inferred due to the now missing color property
    if (asserted != 8 || retracted != 2) return 4;

    return 0;
}
