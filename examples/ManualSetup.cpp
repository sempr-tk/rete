#include <iostream>
#include <fstream>
#include <memory>

#include "../rete-core/ReteCore.hpp"
#include "../rete-rdf/ReteRDF.hpp"
#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/AssertedEvidence.hpp"

using namespace rete;


void save(Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}

int main()
{
    Network net;

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

    // join where object of the most recent wme in the token matches the subject of the wme
    TripleAccessor::Ptr acc0(new TripleAccessor(Triple::OBJECT));
    acc0->index() = 0;
    TripleAccessor::Ptr acc1(new TripleAccessor(Triple::SUBJECT));

    auto join = std::make_shared<GenericJoin>();
    join->addCheck(acc0, acc1);
    BetaNode::connect(join, adapter->getBetaMemory(), foo->getAlphaMemory());


    std::unique_ptr<Accessor> accA(new TripleAccessor(Triple::SUBJECT));
    accA->index() = 1;
    std::unique_ptr<Accessor> accC(new TripleAccessor(Triple::OBJECT));
    accC->index() = 0;

    // the consequence: construct (C1.?a  rdfs:subClassOf  C2.?c)
    InferTriple::Ptr infer(new InferTriple(
        std::move(accA),
        "rdfs:subClassOf",
        std::move(accC)
    ));

    // create an AgendaNode for the production
    auto inferNode = std::make_shared<AgendaNode>(infer, net.getAgenda());
    join->getBetaMemory()->addProduction(inferNode);

    // put in some data
    auto t1 = std::make_shared<Triple>("A", "rdfs:subClassOf", "B");
    auto t2 = std::make_shared<Triple>("B", "rdfs:subClassOf", "C");
    auto t3 = std::make_shared<Triple>("C", "rdfs:subClassOf", "D");

    net.getRoot()->activate(t1, PropagationFlag::ASSERT);
    net.getRoot()->activate(t2, PropagationFlag::ASSERT);
    net.getRoot()->activate(t3, PropagationFlag::ASSERT);

    save(net, "manual_setup_add.dot");

    net.getRoot()->activate(t2, PropagationFlag::RETRACT);

    save(net, "manual_setup_retract.dot");

    return 0;
}
