#include <iostream>
#include <fstream>

#include "../rete-rdf/ReteRDF.hpp"

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-reasoner/AssertedEvidence.hpp"
#include "../rete-reasoner/ExplanationToDotVisitor.hpp"


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

Triple::Ptr on(const std::string& n1, const std::string& n2)
{
    Triple::Ptr t(new Triple(n1, "<on>", n2));
    return t;
}


int main()
{
    RuleParser p;
    Reasoner reasoner;


    p.parseRules(
    "[AtTheBottom: (?b <type> <block>), noValue (?b <on> ?c) -> (?b <level> 0)]"
    "[OnAnother:   (?a <on> ?b), (?b <level> ?lb), sum(?la ?lb 1) -> (?a <level> ?la)]",
        reasoner.net()
    );

    Evidence::Ptr evidence(new AssertedEvidence("fact-group-1"));

    reasoner.addEvidence(block("a0"), evidence);
    reasoner.addEvidence(block("a1"), evidence);
    reasoner.addEvidence(block("a2"), evidence);
    reasoner.addEvidence(block("a3"), evidence);
    reasoner.addEvidence(block("a4"), evidence);

    reasoner.addEvidence(block("b0"), evidence);
    reasoner.addEvidence(block("b1"), evidence);
    reasoner.addEvidence(block("b2"), evidence);

    reasoner.addEvidence(on("a1", "a0"), evidence);
    reasoner.addEvidence(on("a2", "a1"), evidence);
    reasoner.addEvidence(on("a3", "a2"), evidence);
    reasoner.addEvidence(on("a4", "a3"), evidence);

    reasoner.addEvidence(on("b1", "b0"), evidence);
    reasoner.addEvidence(on("b2", "b1"), evidence);


    reasoner.performInference();

    save(reasoner.net(), "blockstower.dot");

    rete::ExplanationToDotVisitor visitor;
    WME::Ptr toExplain(new Triple("a4", "<level>", "4.000000"));
    reasoner.getCurrentState().traverseExplanation(toExplain, visitor);

    {
        std::ofstream towerExplanation("towerExplanation.dot");
        towerExplanation << visitor.str(rete::ExplanationToDotVisitor::FORCE_LOWEST_RANK);
        towerExplanation.close();
    }

    visitor.reset();
    visitor.setMaxDepth(3);
    reasoner.getCurrentState().traverseExplanation(toExplain, visitor);
    {
        std::ofstream towerExplanation("towerExplanation_limited.dot");
        towerExplanation << visitor.str(rete::ExplanationToDotVisitor::FORCE_LOWEST_RANK);
        towerExplanation.close();
    }

    return 0;
}
