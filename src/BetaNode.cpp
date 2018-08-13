#include "../include/BetaNode.hpp"
#include "../include/AlphaMemory.hpp"

namespace rete {

BetaNode::BetaNode(BetaMemory::Ptr beta, AlphaMemory::Ptr alpha)
    : parentAlpha_(alpha), parentBeta_(beta), bmem_(new BetaMemory())
{
}

BetaMemory::Ptr BetaNode::getBetaMemory() const
{
    return bmem_;
}

} /* rete */
