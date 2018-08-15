#include <iostream>

#include "../include/Rete.hpp"

using namespace rete;

/**
    Multiple additions of the same (or equal) WME should not result in multiple entries in the
    alpha memories.
*/
int main()
{
    Network net;

    /*
        (predicate == "self"?) ____
                                   ` (unconditional join)
        (predicate == "color"?) ---'
    */

    auto root = net.getRoot();
    TripleAlpha::Ptr a1(new TripleAlpha(Triple::PREDICATE, "self")); root->addChild(a1);
    TripleAlpha::Ptr b1(new TripleAlpha(Triple::PREDICATE, "color")); root->addChild(b1);
    a1->initAlphaMemory();
    b1->initAlphaMemory();

    AlphaBetaAdapter::Ptr ab(new AlphaBetaAdapter());
    BetaNode::connect(ab, nullptr, a1->getAlphaMemory());

    JoinNode::Ptr j1(new JoinNode());
    BetaNode::connect(j1, ab->getBetaMemory(), b1->getAlphaMemory());


    // add knowledge
    Triple::Ptr t1(new Triple("B1", "self", "B1"));
    Triple::Ptr t2a(new Triple("B1", "color", "red"));
    Triple::Ptr t2b(new Triple("B1", "color", "red"));

    // duplicate: same instance
    root->activate(t1, rete::ASSERT);
    root->activate(t1, rete::ASSERT);
    if (a1->getAlphaMemory()->size() != 1) return 1;

    root->activate(t2a, rete::ASSERT);
    root->activate(t2b, rete::ASSERT);
    if (b1->getAlphaMemory()->size() != 1) return 2;


    return 0;
}
