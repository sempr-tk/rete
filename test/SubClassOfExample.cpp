#include <iostream>
#include <fstream>

#include "../include/Network.hpp"
#include "../include/Triple.hpp"
#include "../include/TripleAlpha.hpp"
#include "../include/AlphaMemory.hpp"
#include "../include/JoinNode.hpp"
#include "../include/AlphaBetaAdapter.hpp"
#include "../include/TripleConsistency.hpp"
#include "../include/TripleJoin.hpp"
#include "../include/AgendaNode.hpp"
#include "../include/InferTriple.hpp"

#include "../include/Reasoner.hpp"
#include "../include/AssertedEvidence.hpp"

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
    Network& net = reasoner.net();

    // setup network
    //        C1                     C2
    // (?a rdfs:subClassOf ?b) (?b rdfs:subClassOf ?c)

    // predicate check
    auto foo = std::make_shared<TripleAlpha>(Triple::PREDICATE, "rdfs:subClassOf");
    net.getRoot()->addChild(foo);
    foo->initAlphaMemory();

    // adapter for first (and only) join
    auto adapter = std::make_shared<AlphaBetaAdapter>();
    BetaNode::connect(adapter, nullptr, foo->getAlphaMemory());

    // join where object of the most recent wme in the token matches the subject of the wme in the
    //bin                                    |--- C1.?b ------|----- C2.?b -----|
    auto join = std::make_shared<TripleJoin>(0, Triple::OBJECT, Triple::SUBJECT);
    BetaNode::connect(join, adapter->getBetaMemory(), foo->getAlphaMemory());


    // the consequence: construct (C1.?a  rdfs:subClassOf  C2.?c)
    InferTriple::Ptr infer(new InferTriple(
        {1, Triple::SUBJECT},
        "rdfs:subClassOf",
        {0, Triple::OBJECT}
    ));

    // alternative construction
    // auto infer = std::make_shared<InferTriple>(
    //     InferTriple::ConstructHelper{1, Triple::SUBJECT},
    //     "rdfs:subClassOf",
    //     InferTriple::ConstructHelper{0, Triple::OBJECT}
    // );

    // create an AgendaNode for the production
    auto inferNode = std::make_shared<AgendaNode>(infer, net.getAgenda());
    join->getBetaMemory()->addProduction(inferNode);

    // put in some data
    auto t1 = std::make_shared<Triple>("A", "rdfs:subClassOf", "B");
    auto t2 = std::make_shared<Triple>("B", "rdfs:subClassOf", "C");
    auto t3 = std::make_shared<Triple>("C", "rdfs:subClassOf", "D");

    AssertedEvidence::Ptr source(new AssertedEvidence("some named graph or just anything"));
    reasoner.addEvidence(t1, source);
    reasoner.addEvidence(t2, source);
    reasoner.addEvidence(t3, source);



    save(net, "0.dot");  // before inference
    reasoner.performInference();
    save(net, "1.dot");  // after inference
    reasoner.removeEvidence(t2, source); // remove "B foo C"
    reasoner.performInference();
    save(net, "2.dot"); // after removing t2

    return 0;
}