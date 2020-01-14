#include <iostream>

#include "../rete-core/ReteCore.hpp"
#include "../rete-rdf/ReteRDF.hpp"

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
    TripleTypeAlpha::Ptr typeCheck(new TripleTypeAlpha()); SetParent(root, typeCheck);

    TripleAlpha::Ptr a1(new TripleAlpha(Triple::PREDICATE, "self")); SetParent(typeCheck, a1);
    TripleAlpha::Ptr b1(new TripleAlpha(Triple::PREDICATE, "color")); SetParent(typeCheck, b1);

    auto a1mem = std::make_shared<AlphaMemory>();
    auto b1mem = std::make_shared<AlphaMemory>();
    SetParent(a1, a1mem);
    SetParent(b1, b1mem);

    AlphaBetaAdapter::Ptr ab(new AlphaBetaAdapter());
    SetParents(nullptr, a1mem, ab);
    auto abmem = std::make_shared<BetaMemory>();
    SetParent(ab, abmem);

    JoinNode::Ptr j1(new GenericJoin());
    SetParents(abmem, b1mem, j1);
    auto j1mem = std::make_shared<BetaMemory>();
    SetParent(j1, j1mem);


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
