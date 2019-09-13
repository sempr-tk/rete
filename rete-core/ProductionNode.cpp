#include "ProductionNode.hpp"
#include "BetaMemory.hpp"

#include <iostream>
rete::ProductionNode::ProductionNode(Production::Ptr p)
    : production_(p), name_("")
{
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

std::string rete::ProductionNode::getDOTAttr() const
{
    return "[label=\"ProductionNode \'" + getName() + "\'\\n" + production_->getName() + "\"]";
}
