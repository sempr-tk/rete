#include <iostream>
#include <fstream>
#include <memory>

#include "../rete-core/ReteCore.hpp"
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

int main()
{
    Reasoner reasoner;
    RuleParser parser;

    parser.parseRules(
        "[subclassTransitive: (?a <subClassOf> ?b), (?b <subClassOf> ?c) -> (?a <subClassOf> ?c)]"
        "[subClassType:       (?a <subClassOf> ?b), (?x <type> ?a) -> (?x <type> ?b)]",
        reasoner.net()
    );

    // put in some data
    auto t1 = std::make_shared<Triple>("A", "<subClassOf>", "B");
    auto t2 = std::make_shared<Triple>("B", "<subClassOf>", "C");
    auto t3 = std::make_shared<Triple>("C", "<subClassOf>", "D");
    auto t4 = std::make_shared<Triple>("x", "<type>", "A");

    AssertedEvidence::Ptr source(new AssertedEvidence("fact-group-1"));
    reasoner.addEvidence(t1, source);
    reasoner.addEvidence(t2, source);
    reasoner.addEvidence(t3, source);
    reasoner.addEvidence(t4, source);

    reasoner.performInference();
    save(reasoner.net(), "subClassOf2Network.dot");

    // explain (x <type> D)
    auto toExplain = std::make_shared<Triple>("x", "<type>", "C");
    ExplanationToDotVisitor visitor;
    reasoner.getCurrentState().traverseExplanation(toExplain, visitor);

    {
        std::ofstream expl("subClassOf2Explanation.dot");
        //expl << visitor.str(ExplanationToDotVisitor::FORCE_LOWEST_RANK);
        expl << visitor.str();
        expl.close();
    }

    {
        for (int i = 0; i < 5; i++)
        {
            visitor.reset();
            visitor.setMaxDepth(i);
            reasoner.getCurrentState().traverseExplanation(toExplain, visitor);

            std::ofstream expl("subClassOf2Explanation_limited" + std::to_string(i) + ".dot");
            //expl << visitor.str(ExplanationToDotVisitor::FORCE_LOWEST_RANK);
            expl << visitor.str();
            expl.close();
        }
    }

    return 0;
}
