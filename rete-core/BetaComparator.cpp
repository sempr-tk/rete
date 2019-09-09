#include <vector>

#include "BetaComparator.hpp"

#include <iostream>

namespace rete {

bool BetaComparator::operator() (const BetaNode::WPtr& aw, const BetaNode::WPtr& bw)
{
    auto a = aw.lock();
    auto b = bw.lock();

    // question: is "a" a descendent of "b"?
    std::vector<BetaNode::Ptr> toVisit;
    toVisit.push_back(b);

    while (!toVisit.empty())
    {
        auto last = toVisit.back();
        toVisit.pop_back();

        // check if we found "a" as descendent of "b"
        if (last == a) return true;

        // else: continue the search.
        auto mem = last->getBetaMemory();
        if (mem)
        {
            std::vector<BetaNode::Ptr> children;
            mem->getChildren(children);
            toVisit.insert(toVisit.end(), children.begin(), children.end());
        }
    }

    return false;
}

} /* rete */
