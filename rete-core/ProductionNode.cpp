#include "ProductionNode.hpp"

#include <iostream>
rete::ProductionNode::ProductionNode(Production::Ptr p)
    : production_(p), name_("")
{
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

void rete::ProductionNode::tearDown()
{
    // the production node has no children, just a list of productions -- but they are not part
    // of the network. So just ignore them.
}
