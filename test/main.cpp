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

using namespace rete;

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

    // put in some data
    auto t1 = std::make_shared<Triple>("A", "foo", "B");
    auto t2 = std::make_shared<Triple>("B", "foo", "C");
    auto t3 = std::make_shared<Triple>("C", "foo", "D");

    net.getRoot()->activate(t1, rete::ASSERT);
    net.getRoot()->activate(t3, rete::ASSERT);
    net.getRoot()->activate(t2, rete::ASSERT);

    if (saveToFile)
    {
        std::ofstream file(filename);
        file << net.toDot();
        file.close();
    }

    return 0;
}
