#include <memory>
#include "connect.hpp"
#include "AlphaNode.hpp"
#include "AlphaMemory.hpp"
#include "BetaNode.hpp"
#include "BetaMemory.hpp"
#include "ProductionNode.hpp"

namespace rete {

// AlphaNode <- AlphaNode
void SetParent(AlphaNode::Ptr parent, AlphaNode::Ptr child)
{
    if (child->parent_) child->parent_->removeChild(child);
    if (parent) parent->addChild(child);
    child->parent_ = parent;
}

// AlphaNode <- AlphaMemory
void SetParent(AlphaNode::Ptr parent, AlphaMemory::Ptr child)
{
    if (parent->amem_.lock()) throw std::exception(); // new parent already has a child amem!

    if (child->parent_)
    {
        child->parent_->amem_.reset();
        child->wmes_.clear(); // TODO RETRACT?
    }

    child->parent_ = parent;
    if (parent) parent->amem_ = child;

    child->initialize();
}

// BetaNode <- BetaMemory
void SetParent(BetaNode::Ptr parent, BetaMemory::Ptr child)
{
    if (parent->bmem_.lock()) throw std::exception(); // new parent already has a bmem!

    if (child->parent_)
    {
        child->parent_->bmem_.reset();
        child->tokens_.clear(); // TODO: retract?
    }

    child->parent_ = parent;
    if (parent) parent->bmem_ = child;

    child->initialize();
}

// (left) BetaMemory <----.
//                        +-- BetaNode
// (right) AlphaMemory <--'
void SetParents(BetaMemory::Ptr bmem, AlphaMemory::Ptr amem, BetaNode::Ptr node)
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

// BetaMemory <- ProductionNode
void SetParent(BetaMemoryPtr parent, ProductionNode::Ptr child)
{
    if (child->parent_)
    {
        child->parent_->removeProduction(child);
        for (auto entry : *child->parent_)
        {
            child->activate(entry, PropagationFlag::RETRACT);
        }
    }

    child->parent_ = parent;
    if (parent) parent->addProduction(child);
    child->initialize();
}


} /* rete */
