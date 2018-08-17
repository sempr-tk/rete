#include "../include/ProductionNode.hpp"

#include <iostream>
rete::ProductionNode::ProductionNode(Production::Ptr p)
    : production_(p)
{
}


std::string rete::ProductionNode::getDOTAttr() const
{
    return "[label=\"ProductionNode\\n" + production_->getName() + "\"]";
}
