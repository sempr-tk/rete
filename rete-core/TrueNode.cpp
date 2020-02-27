#include "TrueNode.hpp"
#include "Util.hpp"

namespace rete {

TrueNode::TrueNode()
    : true_(new EmptyWME())
{
    // by setting the isComputed_ flag we signal that this WME does not need
    // an evidence in the reasoner to hold. Without this, the reasoner would
    // not keep information that is inferred from a rule that makes use of
    // this EmptyWME.
    true_->isComputed_ = true;
}

std::string TrueNode::getDOTAttr() const
{
    return "[label=\"true\"]";
}

void TrueNode::activate(WME::Ptr, PropagationFlag)
{
    // Nothing to do. We don't care about any other WMEs aside from our own
    // internal EmptyWME
}

void TrueNode::initialize()
{
    // No need to initialize this TrueNode.
    // This method is only relevant when it is called by its child AlphaMemory,
    // in which case it just needs to propagate true_:
    propagate(true_, PropagationFlag::ASSERT);
}

bool TrueNode::operator==(const AlphaNode& other) const
{
    // There's no magic in this node, all of them are the same.
    const TrueNode* o = dynamic_cast<const TrueNode*>(&other);
    if (o)
    {
        return true;
    }
    return false;
}

}
