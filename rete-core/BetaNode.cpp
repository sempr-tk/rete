#include "BetaNode.hpp"
#include "AlphaMemory.hpp"
#include "BetaMemory.hpp"

namespace rete {

BetaNode::BetaNode()
{
}

void BetaNode::initialize()
{
    if (parentBeta_)
    {
        for (auto entry : *parentBeta_)
        {
            leftActivate(entry, PropagationFlag::ASSERT);
        }
    }
    else if (parentAlpha_)
    {
        for (auto entry : *parentAlpha_)
        {
            rightActivate(entry, PropagationFlag::ASSERT);
        }
    }
}

BetaMemory::Ptr BetaNode::getParentBeta() const
{
    return parentBeta_;
}

AlphaMemory::Ptr BetaNode::getParentAlpha() const
{
    return parentAlpha_;
}

BetaMemory::Ptr BetaNode::getBetaMemory() const
{
    return bmem_.lock();
}

std::string BetaNode::getDOTAttr() const
{
    return "[label=BetaNode]";
}

std::string BetaNode::toString() const
{
    return "BetaNode";
}


} /* rete */
