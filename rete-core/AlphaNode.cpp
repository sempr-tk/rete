#include <algorithm>

#include "AlphaNode.hpp"
#include "AlphaMemory.hpp"
#include "Util.hpp"

namespace rete {

void AlphaNode::initialize()
{
    if (this->parent_)
    {
        auto amem = this->parent_->amem_.lock();
        if (amem)
        {
            for (auto entry : *amem)
            {
                this->activate(entry, PropagationFlag::ASSERT);
            }
        }
        else
        {
            // parent has no alpha memory?
            // make sure we are the only child of parent, and initialize parent.
            std::vector<AlphaNode::WPtr> childrenBackup = this->parent_->children_;
            auto newEnd = std::remove_if(parent_->children_.begin(), parent_->children_.end(),
                [this](AlphaNode::WPtr entry)
                {
                    auto node = entry.lock();
                    return !node || node.get() != this;
                }
            );
            parent_->children_.erase(newEnd, parent_->children_.end());

            // now initialize the parent
            parent_->initialize();

            // and undo the changes to its children.
            parent_->children_ = childrenBackup;
        }
    }
}


std::string AlphaNode::toString() const
{
    return "AlphaNode";
}


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
