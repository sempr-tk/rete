#include <iostream>
#include <fstream>

#include "../rete-rdf/ReteRDF.hpp"
#include "../rete-reasoner/AssertedEvidence.hpp"

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"


using namespace rete;


void save(Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}

Triple::Ptr block(const std::string& name)
{
    Triple::Ptr t(new Triple(name, "<type>", "<block>"));
    return t;
}

Triple::Ptr red(const std::string& name)
{
    Triple::Ptr t(new Triple(name, "<color>", "<red>"));
    return t;
}

Triple::Ptr leftof(const std::string& n1, const std::string& n2)
{
    Triple::Ptr t(new Triple(n1, "<left-of>", n2));
    return t;
}

Triple::Ptr on(const std::string& n1, const std::string& n2)
{
    Triple::Ptr t(new Triple(n1, "<on>", n2));
    return t;
}

Triple::Ptr size(const std::string& name, float size)
{
    Triple::Ptr t(new Triple(name, "<edge-size>", std::to_string(size)));
    return t;
}

int main()
{
    RuleParser p;
    Reasoner reasoner;


    auto rules = p.parseRules(
"[OnARedBlock: (?b1 <on> ?b2), (?b2 <color> <red>) -> (?b1 <on-red-block> ?b2)]\n"
"[LeftOfANotRedBlock:"
    "(?b1 <left-of> ?b2), noValue { (?b2 <color> <red>) } -> (?b1 <left-of-not-red-block> ?b2)]\n"
"[BlockVolume:"
    "(?b <type> <block>), (?b <edge-size> ?len), mul(?volume ?len ?len ?len)"
    "-> (?b <volume> ?volume)]",
        reasoner.net()
    );

    Evidence::Ptr evidence(new AssertedEvidence("fact-group-1"));

    reasoner.addEvidence(block("a0"), evidence);
    reasoner.addEvidence(block("a1"), evidence);
    reasoner.addEvidence(block("b0"), evidence);
    reasoner.addEvidence(block("b1"), evidence);
    reasoner.addEvidence(block("c"), evidence);
    reasoner.addEvidence(block("d"), evidence);

    reasoner.addEvidence(on("a1", "a0"), evidence);
    reasoner.addEvidence(on("b0", "b1"), evidence);

    reasoner.addEvidence(red("a0"), evidence);
    reasoner.addEvidence(red("c"), evidence);

    reasoner.addEvidence(leftof("c", "a0"), evidence);
    reasoner.addEvidence(leftof("d", "c"), evidence);
    reasoner.addEvidence(leftof("d", "b0"), evidence);

    reasoner.addEvidence(size("a0", 1.451), evidence);
    reasoner.addEvidence(size("b0", 2), evidence);
    reasoner.addEvidence(size("c", 3), evidence);
    reasoner.addEvidence(size("d", 4), evidence);


    reasoner.performInference();

    save(reasoner.net(), "blocksworld.dot");

    return 0;
}
