#include <iostream>
#include <fstream>

#include <rete-core/ReteCore.hpp>
#include <rete-rdf/ReteRDF.hpp>

#include <rete-reasoner/Reasoner.hpp>
#include <rete-reasoner/AssertedEvidence.hpp>

#include <rete-reasoner/RuleParser.hpp>

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
    // (?x foo ?y) (?y foo ?z)

    auto foo = std::make_shared<TripleAlpha>(Triple::PREDICATE, "foo");
    net.getRoot()->addChild(foo);
    foo->initAlphaMemory();

    auto adapter = std::make_shared<AlphaBetaAdapter>();
    BetaNode::connect(adapter, nullptr, foo->getAlphaMemory());

    // auto join = std::make_shared<TripleJoin>(0, Triple::OBJECT, Triple::SUBJECT);
    GenericJoin<std::string>::Ptr join(new GenericJoin<std::string>());
    // setup generic join criteria:
    auto accessor1 = std::make_shared<TripleAccessor>(Triple::OBJECT);
    accessor1->index() = 0;
    auto accessor2 = std::make_shared<TripleAccessor>(Triple::SUBJECT);

    join->addCheck(accessor1, accessor2);


    BetaNode::connect(join, adapter->getBetaMemory(), foo->getAlphaMemory());

    // put in some data
    auto t1 = std::make_shared<Triple>("A", "foo", "B");
    auto t2 = std::make_shared<Triple>("B", "foo", "C");
    auto t3 = std::make_shared<Triple>("C", "foo", "D");

    net.getRoot()->activate(t1, rete::ASSERT);
    net.getRoot()->activate(t3, rete::ASSERT);
    net.getRoot()->activate(t2, rete::ASSERT);


    // output
    save(net, "genericjoin.dot");

    // there shall be only two matches:
    // [(A foo B) (B foo C)]
    // and
    // [(B foo C) (C foo D)]
    if (join->getBetaMemory()->size() == 2) return 0;
    return 1;

    return 0;
}
