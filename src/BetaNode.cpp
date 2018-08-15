#include "../include/BetaNode.hpp"
#include "../include/AlphaMemory.hpp"

namespace rete {

BetaNode::BetaNode()
    : bmem_(new BetaMemory())
{
}

void BetaNode::connect(BetaNode::Ptr node, BetaMemory::Ptr bmem, AlphaMemory::Ptr amem)
{
    node->parentAlpha_ = amem;
    node->parentBeta_ = bmem;

    // NOTE: Order matters!
    // The child entities of the AlphaMemory are sorted, with descendants < ancestors.
    // After adding node as a child of bmem may become a descendant of amem, but amems children are
    // only sorted on insertion.
    if (bmem) bmem->addChild(node);
    if (amem) amem->addChild(node);
}


BetaMemory::Ptr BetaNode::getBetaMemory() const
{
    return bmem_;
}

std::string BetaNode::getDOTAttr() const
{
    return "[label=BetaNode]";
}

} /* rete */
