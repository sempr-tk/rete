#include "ProductionNode.hpp"
#include "BetaMemory.hpp"

#include <iostream>
rete::ProductionNode::ProductionNode(Production::Ptr p)
    : production_(p), name_("")
{
}

rete::ProductionNode::~ProductionNode()
{
    if (parent_) parent_->removeProduction(this);
}

void rete::ProductionNode::initialize()
{
    if (parent_)
    {
        for (auto entry : *parent_)
        {
            activate(entry, PropagationFlag::ASSERT);
        }
    }
}

void rete::ProductionNode::setName(const std::string& name)
{
    name_ = name;
}

std::string rete::ProductionNode::getName() const
{
    return name_;
}


rete::Production::Ptr rete::ProductionNode::getProduction() const
{
    return this->production_;
}

std::string rete::ProductionNode::getDOTAttr() const
{
    return "[label=\"ProductionNode \'" + getName() + "\'\\n" + production_->getName() + "\"]";
}

std::string rete::ProductionNode::toString() const
{
    return "ProductionNode \'" + getName() + "\'\n" + production_->getName();
}
