#include <iostream>

#include "../include/Network.hpp"
#include "../include/Triple.hpp"
#include "../include/TripleAlpha.hpp"
#include "../include/AlphaMemory.hpp"

int main()
{
    // rete::Network net;

    // setup network

    rete::TripleAlpha::Ptr test1(new rete::TripleAlpha(rete::Triple::PREDICATE, "color"));
    rete::TripleAlpha::Ptr test2(new rete::TripleAlpha(rete::Triple::OBJECT, "red"));
    test1->addChild(test2);

    test1->initAlphaMemory();
    test2->initAlphaMemory();

    // add some data
    rete::Triple::Ptr t1(new rete::Triple("s1", "color", "red"));
    rete::Triple::Ptr t2(new rete::Triple("s2", "color", "blue"));
    rete::Triple::Ptr t3(new rete::Triple("s1", "likes", "red"));

    test1->activate(t1);
    test1->activate(t2);
    test1->activate(t3);


    return 0;
}
