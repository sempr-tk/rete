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

using namespace rete;


class DummyProduction : public Production {
    void execute(Token::Ptr token) override
    {
        std::cout << "DummyProduction activated with token " << token->toString() << std::endl;
    }

    std::string getName() const override
    {
        return "Dummy";
    }
};


int main(int argc, char** args)
{
    // std::cout << argc << std::endl;
    // for (int i = 0; i < argc; i++)
    // {
    //     std::cout << args[i] << std::endl;
    // }

    bool saveToFile = false;
    std::string filename = "";
    if (argc == 2)
    {
        saveToFile = true;
        filename = args[1];
    }

    Network net;


    // setup network
    // (?x foo ?y) (?y foo ?z)

    auto foo = std::make_shared<TripleAlpha>(Triple::PREDICATE, "foo");
    net.getRoot()->addChild(foo);
    foo->initAlphaMemory();

    auto adapter = std::make_shared<AlphaBetaAdapter>();
    BetaNode::connect(adapter, nullptr, foo->getAlphaMemory());

    auto join = std::make_shared<TripleJoin>(0, Triple::OBJECT, Triple::SUBJECT);
    BetaNode::connect(join, adapter->getBetaMemory(), foo->getAlphaMemory());


    auto dummy = std::make_shared<DummyProduction>();
    auto dummyNode = std::make_shared<AgendaNode>(dummy, net.getAgenda());

    join->getBetaMemory()->addProduction(dummyNode);

    // put in some data
    auto t1 = std::make_shared<Triple>("A", "foo", "B");
    auto t2 = std::make_shared<Triple>("B", "foo", "C");
    auto t3 = std::make_shared<Triple>("C", "foo", "D");

    net.getRoot()->activate(t1, rete::ASSERT);
    net.getRoot()->activate(t3, rete::ASSERT);
    net.getRoot()->activate(t2, rete::ASSERT);


    auto agenda = net.getAgenda();
    if (agenda->empty()) std::cout << "Agenda is empty." << std::endl;
    while (!agenda->empty())
    {
        auto item = agenda->front();
        agenda->pop_front();

        std::get<1>(item)->execute(std::get<0>(item));
    }


    if (saveToFile)
    {
        std::ofstream file(filename);
        file << net.toDot();
        file.close();
    }

    return 0;
}
