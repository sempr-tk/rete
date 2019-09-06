#include "ProductionNode.hpp"
#include "BetaMemory.hpp"

#include <iostream>
rete::ProductionNode::ProductionNode(Production::Ptr p)
    : production_(p), name_("")
{
}

void rete::SetParent(BetaMemoryPtr parent, ProductionNode::Ptr child)
{
    if (child->parent_) child->parent_->removeProduction(child);
    child->parent_ = parent;
    parent->addProduction(child);
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
