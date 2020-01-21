#include "AlphaMemory.hpp"
#include "AlphaNode.hpp"
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
            WME::Ptr w = *it;
            wmes_.erase(it);
            // only propagate if actually removed, and only propagate exactly
            // what was removed, not the probably equivalent but different instance
            // since some checks later on (BetaMemory e.g.) just compare
            // pointer. The AlphaMemory is the layer that sorts out the
            // duplicates.
            propagate(w, flag);
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
        auto it = wmes_.find(wme);
        if (it != wmes_.end())
        {
            // has been a match before --> UPDATE
            // NOTE: We do *not* propagate "wme" here, but "*it", because the
            // given "wme" might be a different instance that evaluates to the
            // same *value* (see WMEComparator!), but holds the exact same
            // mutable component that was added previously in another WME.
            // This might sound a bit complicated, so let me explain the exact
            // situation here:
            // Assume you have a WME that is a Tuple<A*, std::string>, where
            // the value A that is pointed at is mutable. You at some point
            // added such a Tuple<A*, std::string> as a WME to the rete network
            // and now get informed from an external source that the pointed at
            // "A" has changed. But you do not have the original
            // Tuple<A*, std::string>::Ptr you added at first anymore!
            // So you create a new Tuple<A*, std::string> and use this instead:
            // Since it point to the exact same A and has the same std::string,
            // it is equal by value to the old Tuple<...> and thus the above
            // wmes_.find(wme) returns the old Tuple<...>::Ptr!
            //
            // It is already the case that the part of mutable wmes that is
            // considered in the by-value-comparison must be constant, or else
            // it will mess up our std::set<WME::Ptr>. So the "by-value" can be
            // rather interpreted as "the data that is identified by this".
            //
            // But it is not completely irrelevant whether we use *it or wme,
            // because the tokens in the beta memories are partly evaluated by
            // ptr. And the WME that is pointed at is actually a different one,
            // though it has the same content as the one added before.
            propagate(*it, PropagationFlag::UPDATE);
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
        auto c = child.lock();
        if (c) c->rightActivate(wme, flag);
    }
}

void AlphaMemory::addChild(BetaNode::Ptr beta)
{
    children_.push_back(beta);

    // sort to avoid duplicate tokens in join nodes!
    std::sort(children_.begin(), children_.end(), BetaComparator());
}

void AlphaMemory::removeChild(BetaNode::WPtr child)
{
    children_.erase(
        std::find_if(children_.begin(), children_.end(), util::EqualWeak<BetaNode>(child)),
        children_.end()
    );
}


void AlphaMemory::getChildren(std::vector<BetaNode::Ptr>& children)
{
    children.reserve(children_.size());
    for (auto c : children_)
    {
        auto child = c.lock();
        if (child) children.push_back(child);
    }
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


void AlphaMemory::initialize()
{
    if (parent_)
    {
        // remove all children -- only this memory node shall get the activations.
        std::vector<AlphaNode::WPtr> backup;
        backup.swap(parent_->children_);

        parent_->initialize();

        // reset children
        backup.swap(parent_->children_);
    }
}

} /* rete */
