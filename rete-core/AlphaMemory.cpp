#include "AlphaMemory.hpp"
#include "BetaComparator.hpp"
#include "Util.hpp"

#include <algorithm>
#include <iostream> // debug

namespace rete {

void AlphaMemory::activate(WME::Ptr wme, PropagationFlag flag)
{
    if (flag == PropagationFlag::ASSERT)
    {
        auto result = wmes_.insert(wme);
        if (result.second)
        {
            // only propagate insertion of actually inserted (no duplicate!)
            propagate(wme, flag);
        }
    }
    else if (flag == PropagationFlag::RETRACT)
    {
        auto it = wmes_.find(wme);
        if (it != wmes_.end())
        {
            wmes_.erase(it);
            // only propagate if actually removed
            propagate(wme, flag);
        }
    }
    else if (flag == PropagationFlag::UPDATE)
    {
        /**
            UPDATEs are a difficult to handle: Does a match still hold? Then UPDATE must be
            propagated. Does it match only since the change? Then it is actually an ASSERT. Or does
            it no longer match? RETRACT, then.
            Since the Alpha/Beta-*Memory* nodes hold the previous matches and can thus decide
            whether a match is new etc., the strategy to deal with UPDATEs is the following:
            Alpha/Beta-*Node*s do their checks on an UPDATE as usual, and propagate either RETRACT
            if it does not match or UPDATE if it matches. The next Memory-node then decides if an
            UPDATE is a true UPDATE or an ASSERT and propagates the event accordingly.
            NOTE: This also means that join nodes must re-evaluate all combinations with the
            UPDATEd token/wme and *explicitely* propagate RETRACTs.
        */
        if (wmes_.find(wme) != wmes_.end())
        {
            // has been a match before --> UPDATE
            propagate(wme, PropagationFlag::UPDATE);
        }
        else
        {
            // has not been a match before --> ASSERT
            propagate(wme, PropagationFlag::ASSERT);
        }
    }
}

void AlphaMemory::propagate(WME::Ptr wme, PropagationFlag flag)
{
    for (auto child : children_)
    {
        child->rightActivate(wme, flag);
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

std::string AlphaMemory::getDOTAttr() const
{
    std::string record = "";
    for (auto w : wmes_)
    {
        record += "|" + util::dotEscape(w->toString());
    }
    return "[shape=record, label=\"{AlphaMemory" + record + "}\"]";
}


void AlphaMemory::tearDown()
{
    for (auto child : children_)
    {
        child->tearDown();
    }
    children_.clear();
}

} /* rete */
