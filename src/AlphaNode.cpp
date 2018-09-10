#include "../include/AlphaNode.hpp"
#include "../include/AlphaMemory.hpp"

namespace rete {

void AlphaNode::addChild(AlphaNode::Ptr node)
{
    children_.push_back(node);
}

void AlphaNode::getChildren(std::vector<AlphaNode::Ptr>& children)
{
    children.reserve(children_.size());
    for (auto c : children_) children.push_back(c);
}


void AlphaNode::propagate(WME::Ptr wme, PropagationFlag flag)
{
    for (auto child :children_)
    {
        child->activate(wme, flag);
    }

    if (amem_) amem_->activate(wme, flag);
}

bool AlphaNode::hasAlphaMemory() const
{
    return amem_.get();
}

void AlphaNode::initAlphaMemory()
{
    if (hasAlphaMemory()) return;
    amem_.reset(new AlphaMemory());

    // TODO: Initialize with known WMEs? Currently don't support adding rules on the fly.
}

AlphaMemory::Ptr AlphaNode::getAlphaMemory() const
{
    return amem_;
}

std::string AlphaNode::getDOTAttr() const
{
    return "[label=AlphaNode]";
}

void AlphaNode::tearDown()
{
    for (auto child : children_)
    {
        child->tearDown();
    }
    children_.clear();

    if (amem_)
    {
        amem_->tearDown();
        amem_.reset();
    }

}

} /* rete */
