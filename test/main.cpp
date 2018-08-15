#include <iostream>
#include <fstream>

#include "../include/Network.hpp"
#include "../include/Triple.hpp"
#include "../include/TripleAlpha.hpp"
#include "../include/AlphaMemory.hpp"
#include "../include/JoinNode.hpp"
#include "../include/AlphaBetaAdapter.hpp"
#include "../include/TripleConsistency.hpp"

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
    /*
        Example:
            (?x self ?y)
            (?x color red)
            (?y color red)
    */
    auto root = net.getRoot();
    TripleAlpha::Ptr a1(new TripleAlpha(Triple::PREDICATE, "self")); root->addChild(a1);
    TripleConsistency::Ptr a2(new TripleConsistency(Triple::SUBJECT, Triple::OBJECT));
    a1->addChild(a2);

    TripleAlpha::Ptr b1(new TripleAlpha(Triple::PREDICATE, "color")); root->addChild(b1);
    TripleAlpha::Ptr b2(new TripleAlpha(Triple::OBJECT, "red")); b1->addChild(b2);
    a2->initAlphaMemory();
    b2->initAlphaMemory();

    AlphaBetaAdapter::Ptr ab(new AlphaBetaAdapter());
    BetaNode::connect(ab, nullptr, a2->getAlphaMemory());

    JoinNode::Ptr j1(new JoinNode());
    BetaNode::connect(j1, ab->getBetaMemory(), b2->getAlphaMemory());

    JoinNode::Ptr j2(new JoinNode());
    BetaNode::connect(j2, j1->getBetaMemory(), b2->getAlphaMemory());


    // add knowledge
    Triple::Ptr t1(new Triple("B1", "self", "B1"));
    Triple::Ptr t2(new Triple("B1", "color", "red"));

    std::cout << "Activate t1" << std::endl;
    root->activate(t1, rete::ASSERT);
    std::cout << "Activate t2" << std::endl;
    root->activate(t2, rete::ASSERT);

    if (saveToFile)
    {
        std::ofstream file("added_" + filename);
        file << net.toDot();
        file.close();
    }

    // remove by pointer (easy)
    // root->activate(t1, rete::RETRACT);
    // remove by value (hard)
    std::cout << "remove by value" << std::endl;
    // Triple::Ptr tr(new Triple("B1", "self", "B1"));
    Triple::Ptr tr(new Triple("B1", "color", "red"));
    root->activate(tr, rete::RETRACT);

    std::cout << "manual test: " << (*t1 == *tr) << std::endl;

    if (saveToFile)
    {
        std::ofstream file("removed_" + filename);
        file << net.toDot();
        file.close();
    }

    return 0;
}
