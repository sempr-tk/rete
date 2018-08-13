#include "../include/AlphaMemory.hpp"
#include "../include/BetaComparator.hpp"

#include <algorithm>
#include <iostream> // debug

namespace rete {

void AlphaMemory::activate(WME::Ptr wme)
{
    // std::cout << "AlphaMemory " << this << " activated: " << wme.get() << std::endl;
    // TODO: check for duplicates? be aware: currently only ptr-comparison in set!
    wmes_.insert(wme);
    propagate(wme);
}

void AlphaMemory::propagate(WME::Ptr wme)
{
    for (auto child : children_)
    {
        child->rightActivate(wme);
    }
}

void AlphaMemory::addChild(BetaNode::Ptr beta)
{
    children_.push_back(beta);

    // sort to avoid duplicate tokens in join nodes!
    std::sort(children_.begin(), children_.end(), BetaComparator());
}

void AlphaMemory::getChildren(std::vector<BetaNode::Ptr>& children)
{
    children.reserve(children_.size());
    for (auto c : children_) children.push_back(c);
}


size_t AlphaMemory::size() const
{
    return wmes_.size();
}

AlphaMemory::Iterator AlphaMemory::begin()
{
    return wmes_.begin();
}

AlphaMemory::Iterator AlphaMemory::end()
{
    return wmes_.end();
}

} /* rete */
