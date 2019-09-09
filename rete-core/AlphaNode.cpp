#include <algorithm>

#include "AlphaNode.hpp"
#include "AlphaMemory.hpp"
#include "Util.hpp"

namespace rete {

void AlphaNode::addChild(AlphaNode::Ptr node)
{
    children_.push_back(node);
}

void AlphaNode::removeChild(AlphaNode::WPtr node)
{
    children_.erase(
        std::remove_if(children_.begin(), children_.end(), util::EqualWeak<AlphaNode>(node)),
        children_.end()
    );
}


void AlphaNode::getChildren(std::vector<AlphaNode::Ptr>& children)
{
    children.reserve(children_.size());
    for (auto c : children_) {
        auto cl = c.lock();
        if (cl) children.push_back(cl);
        // else the child has been destroyed. Cleanup? TODO
    }
}


void AlphaNode::propagate(WME::Ptr wme, PropagationFlag flag)
{
    for (auto child :children_)
    {
        auto c = child.lock();
        if (c) c->activate(wme, flag);
    }

    auto amem = amem_.lock();
    if (amem) amem->activate(wme, flag);
}


AlphaMemory::Ptr AlphaNode::getAlphaMemory() const
{
    return amem_.lock();
}

std::string AlphaNode::getDOTAttr() const
{
    return "[label=AlphaNode]";
}

} /* rete */
