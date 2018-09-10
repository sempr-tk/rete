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

void rete::ProductionNode::tearDown()
{
    // the production node has no children, just a list of productions -- but they are not part
    // of the network. So just ignore them.
}
