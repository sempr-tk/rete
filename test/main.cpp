#include <iostream>
#include <fstream>

#include "../include/Network.hpp"
#include "../include/Triple.hpp"
#include "../include/TripleAlpha.hpp"
#include "../include/AlphaMemory.hpp"
#include "../include/JoinNode.hpp"
#include "../include/AlphaBetaAdapter.hpp"

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
    TripleAlpha::Ptr b1(new TripleAlpha(Triple::PREDICATE, "color")); root->addChild(b1);
    TripleAlpha::Ptr b2(new TripleAlpha(Triple::OBJECT, "red")); b1->addChild(b2);
    a1->initAlphaMemory();
    b2->initAlphaMemory();

    AlphaBetaAdapter::Ptr ab(new AlphaBetaAdapter(a1->getAlphaMemory()));
    a1->getAlphaMemory()->addChild(ab);

    JoinNode::Ptr j1(new JoinNode(ab->getBetaMemory(), b2->getAlphaMemory()));
    ab->getBetaMemory()->addChild(j1); b2->getAlphaMemory()->addChild(j1);

    JoinNode::Ptr j2(new JoinNode(j1->getBetaMemory(), b2->getAlphaMemory()));
    j1->getBetaMemory()->addChild(j2); b2->getAlphaMemory()->addChild(j2);


    // add stuff
    // (B1 self B1)
    Triple::Ptr t1(new Triple("B1", "self", "B1"));
    // a duplicate
    Triple::Ptr tdup(new Triple("B1", "self", "B1"));
    // (B1 color red)
    Triple::Ptr t2(new Triple("B1", "color", "red"));

    std::cout << "Activate t1" << std::endl;
    root->activate(t1, rete::ASSERT);
    std::cout << "Activate t2" << std::endl;
    root->activate(t2, rete::ASSERT);
    std::cout << "Activate tdup" << std::endl;
    root->activate(tdup, rete::ASSERT);

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
