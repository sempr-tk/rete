#include "BetaNode.hpp"
#include "AlphaMemory.hpp"
#include "BetaMemory.hpp"

namespace rete {

BetaNode::BetaNode()
{
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

} /* rete */
