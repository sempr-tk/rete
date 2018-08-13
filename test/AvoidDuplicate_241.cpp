#include <iostream>

#include "../include/Rete.hpp"

using namespace rete;

/**
    This Test implements the example from chapter "2.4.1 Avoiding Duplicate Tokens"
*/
int main()
{
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


    // add knowledge
    Triple::Ptr t1(new Triple("B1", "self", "B1"));
    Triple::Ptr t2(new Triple("B1", "color", "red"));

    std::cout << "Activate t1" << std::endl;
    root->activate(t1, rete::ASSERT);
    std::cout << "Activate t2" << std::endl;
    root->activate(t2, rete::ASSERT);

    if (j2->getBetaMemory()->size() == 1) return 0;
    return 1;
}
