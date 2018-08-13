#include <iostream>

#include "../include/Network.hpp"
#include "../include/Triple.hpp"
#include "../include/TripleAlpha.hpp"
#include "../include/AlphaMemory.hpp"
#include "../include/JoinNode.hpp"
#include "../include/AlphaBetaAdapter.hpp"

int main()
{
    rete::Network net;

    // setup network

    rete::TripleAlpha::Ptr test1(new rete::TripleAlpha(rete::Triple::PREDICATE, "color"));
    rete::TripleAlpha::Ptr test2(new rete::TripleAlpha(rete::Triple::OBJECT, "red"));
    test1->addChild(test2);

    test1->initAlphaMemory();
    test2->initAlphaMemory();

    net.getRoot()->addChild(test1);


    // TODO: redundant: need Ptr in ctor and set result as child...
    rete::AlphaBetaAdapter::Ptr ab(new rete::AlphaBetaAdapter(test1->getAlphaMemory()));
    test1->getAlphaMemory()->addChild(ab);

    rete::JoinNode::Ptr join(new rete::JoinNode(ab->getBetaMemory(), test2->getAlphaMemory()));
    ab->getBetaMemory()->addChild(join);
    test2->getAlphaMemory()->addChild(join);

    net.getRoot()->initAlphaMemory();
    rete::AlphaBetaAdapter::Ptr abRoot(new rete::AlphaBetaAdapter(net.getRoot()->getAlphaMemory()));
    net.getRoot()->getAlphaMemory()->addChild(abRoot);

    rete::JoinNode::Ptr joinAll(new rete::JoinNode(abRoot->getBetaMemory(),
                                net.getRoot()->getAlphaMemory()));
    abRoot->getBetaMemory()->addChild(joinAll);
    net.getRoot()->getAlphaMemory()->addChild(joinAll);


    // add some data
    rete::Triple::Ptr t1(new rete::Triple("s1", "color", "red"));
    rete::Triple::Ptr t2(new rete::Triple("s2", "color", "blue"));
    rete::Triple::Ptr t3(new rete::Triple("s1", "likes", "red"));

    // test1->activate(t1);
    // test1->activate(t2);
    // test1->activate(t3);
    net.getRoot()->activate(t1);
    net.getRoot()->activate(t2);
    net.getRoot()->activate(t3);


    // check the join node
    // for (auto token : *join->getBetaMemory())
    // {
        // std::cout << token->toString() << std::endl;
    // }

    // std::cout << " ---- join all ---- " << std::endl;
    // for (auto token : *joinAll->getBetaMemory())
    // {
        // std::cout << token->toString() << std::endl;
    // }

    // std::cout << " ---- dot ---- " << std::endl;
    std::cout << net.toDot() << std::endl;

    return 0;
}
