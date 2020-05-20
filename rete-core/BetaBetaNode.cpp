#include "BetaBetaNode.hpp"

#include <iostream>

namespace rete {


std::string BetaBetaNode::getDOTAttr() const
{
    return "[label=\"BetaBetaNode\"]";
}

void BetaBetaNode::initialize()
{
    for (auto token : *parentLeft_)
    {
        this->leftActivate(token, PropagationFlag::ASSERT);
    }
}

BetaBetaNode::BetaBetaNode()
    : rightActivator_(new BetaBetaRightActivator(this))
{
}

BetaBetaNode::~BetaBetaNode()
{
    if (parentRight_) parentRight_->removeChild(rightActivator_);
    if (parentLeft_) parentLeft_->removeChild(this);
}

bool BetaBetaNode::operator==(const BetaNode& other) const
{
    auto optr = dynamic_cast<const BetaBetaNode*>(&other);
    if (!optr) return false;
    return true;
}

void BetaBetaNode::rightActivate(WME::Ptr, PropagationFlag)
{
    throw std::exception();
}

BetaNode::Ptr BetaBetaNode::getRightActivator() const
{
    return rightActivator_;
}


BetaMemory::Ptr BetaBetaNode::getLeftParent() const
{
    return parentLeft_;
}

BetaMemory::Ptr BetaBetaNode::getRightParent() const
{
    return parentRight_;
}

// ---------------------------------------------------------------------------
// BetaBetaRightActivator
// ---------------------------------------------------------------------------

BetaBetaRightActivator::BetaBetaRightActivator(BetaBetaNode* node)
    : node_(node)
{
}
// just delegate stuff to node_
std::string BetaBetaRightActivator::getDOTId() const { return node_->getDOTId(); }
std::string BetaBetaRightActivator::getDOTAttr() const { return node_->getDOTAttr(); }
void BetaBetaRightActivator::initialize() { node_->initialize(); }
void BetaBetaRightActivator::rightActivate(WME::Ptr wme, PropagationFlag flag) { node_->rightActivate(wme, flag); }
bool BetaBetaRightActivator::operator == (const BetaNode& other) const { return *node_ == other; }
BetaMemory::Ptr BetaBetaRightActivator::getBetaMemory() const { return node_->getBetaMemory(); }

// only special feature: map leftActivate to rightActivate
void BetaBetaRightActivator::leftActivate(Token::Ptr token, PropagationFlag flag)
{
    node_->rightActivate(token, flag);
}

}
