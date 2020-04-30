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
        Complex but not nested
    */
    auto rules = p.parseRules(
        "[rule1: (?p <type> <parkinglot>), noValue { (?c <parksOn> ?p), (?c <color> <red>) } -> (<thereis> <nothing-red-on> ?p)]",
        reasoner.net()
    );


    int asserted = 0;
    int retracted = 0;
    auto callback = [&](WME::Ptr wme, PropagationFlag flag)
    {
        std::cout << (flag == PropagationFlag::ASSERT ? "asserted: " : "retracted: ") << wme->toString() << std::endl;
        if (flag == PropagationFlag::ASSERT) asserted++;
        if (flag == PropagationFlag::RETRACT) retracted++;
    };
    reasoner.setCallback(callback);


    Triple::Ptr wme0(new Triple("<p1>","<type>", "<parkinglot>"));
    Triple::Ptr wme1(new Triple("<c1>","<parksOn>", "<p1>"));
    Triple::Ptr wme2(new Triple("<c1>","<color>", "<red>"));
    Triple::Ptr wme3(new Triple("<c1>", "<color>", "<green>"));
    AssertedEvidence::Ptr evidence0(new AssertedEvidence("Evidence0"));
    AssertedEvidence::Ptr evidence1(new AssertedEvidence("Evidence1"));

    reasoner.addEvidence(wme0, evidence0); // add parkinglot
    reasoner.addEvidence(wme1, evidence0); // add c1 on parkinglot
    reasoner.performInference();
    save(reasoner.net(), "novalue_complex_0_init.dot");

    // 2 assertions, +1 "nothing on" = 3
    if (asserted != 3 || retracted != 0) return 2;

    reasoner.addEvidence(wme2, evidence1); // add color red
    reasoner.performInference();
    save(reasoner.net(), "novalue_complex_1_redcar.dot");

    // 1 more assertion, but also 1 retraction due to the negative join on red color
    if (asserted != 4 || retracted != 1) return 3;

    reasoner.removeEvidence(evidence1);    // remove color red
    reasoner.addEvidence(wme3, evidence1); // add color green
    reasoner.performInference();
    save(reasoner.net(), "novalue_complex_2_greencar.dot");

    // retract color red --> add nothing-red-on, add color green
    // --> -1, +2
    if (asserted != 6 || retracted != 2) return 4;

    // reset reasoner
    rules.clear();

    reasoner.performInference();

    // reset counts
    asserted = 0; retracted = 0;

    // current facts:
    // <p1> <type> <parkinglot>
    // <c1> <parksOn> <p1>
    // <c1> <color> <green>

    /**
        Complex, nested
        Trigger if there are no cars on the lot that are not red (-> all red)
    */
    rules = p.parseRules(
        "[rule2: (?p <type> <parkinglot>), noValue { (?c <parksOn> ?p), noValue { (?c <color> <red>) } } -> (<thereis> <no-not-red-on> ?p)]",
        reasoner.net()
    );

    reasoner.performInference(); // should infer nothing.
    save(reasoner.net(), "novalue_complex_nested_0.dot");
    if (asserted != 0 || retracted != 0) return 6;

    // remove the green --> still nothing
    reasoner.removeEvidence(evidence1);
    reasoner.performInference();
    save(reasoner.net(), "novalue_complex_nested_1.dot");

    if (asserted != 0 || retracted != 1) return 7;

    // add the red --> everything on p1 is red
    reasoner.addEvidence(wme2, evidence1);
    reasoner.performInference();
    save(reasoner.net(), "novalue_complex_nested_2.dot");

    if (asserted != 2 || retracted != 1) return 8;

    // remove the red --> nothing. its not red, thats bad.
    reasoner.removeEvidence(evidence1);
    reasoner.performInference();
    save(reasoner.net(), "novalue_complex_nested_3.dot");

    if (asserted != 2 || retracted != 3) return 9;

    // remove the car --> nice, everything (which is nothing) on the lot is red
    reasoner.removeEvidence(wme1, evidence0);
    reasoner.performInference();
    save(reasoner.net(), "novalue_complex_nested_4.dot");

    if (asserted != 3 || retracted != 4) return 10;

    return 0;
}
