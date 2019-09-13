#include <iostream>

#include "../rete-core/ReteCore.hpp"
#include "../rete-rdf/ReteRDF.hpp"

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
    TripleAlpha::Ptr a1(new TripleAlpha(Triple::PREDICATE, "self"));
    SetParent(root, a1);
    TripleConsistency::Ptr a2(new TripleConsistency(Triple::SUBJECT, Triple::OBJECT));
    SetParent(a1, a2);

    TripleAlpha::Ptr b1(new TripleAlpha(Triple::PREDICATE, "color"));
    SetParent(root, b1);
    TripleAlpha::Ptr b2(new TripleAlpha(Triple::OBJECT, "red"));
    SetParent(b1, b2);

    auto a2mem = std::make_shared<AlphaMemory>();
    auto b2mem = std::make_shared<AlphaMemory>();
    SetParent(a2, a2mem);
    SetParent(b2, b2mem);

    AlphaBetaAdapter::Ptr ab(new AlphaBetaAdapter());
    SetParents(nullptr, a2mem, ab);
    auto abmem = std::make_shared<BetaMemory>();
    SetParent(ab, abmem);

    GenericJoin::Ptr j1(new GenericJoin());
    TripleAccessor::Ptr acc0(new TripleAccessor(Triple::SUBJECT));
    acc0->index() = 0;
    TripleAccessor::Ptr acc1(new TripleAccessor(Triple::SUBJECT));
    j1->addCheck(acc0, acc1);

    SetParents(abmem, b2mem, j1);
    auto j1mem = std::make_shared<BetaMemory>();
    SetParent(j1, j1mem);

    GenericJoin::Ptr j2(new GenericJoin());
    SetParents(j1mem, b2mem, j2);
    auto j2mem = std::make_shared<BetaMemory>();
    SetParent(j2, j2mem);

    // j2 keeps everything alive.

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
