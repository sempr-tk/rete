#include "../include/ProductionNode.hpp"

namespace rete {

ProductionNode::ProductionNode(Production::Ptr p)
    : production_(p)
{
}


std::string ProductionNode::getDOTAttr() const
{
    return "[label=\"ProductionNode\\n" + production_->getName() + "\"]";
}


void ProductionNode::activate(Token::Ptr token, PropagationFlag flag)
{
    if (flag == rete::ASSERT)
    {
        agenda_->add({token, production_});
    } else {
        agenda_->remove({token, production_});
    }
}


void ProductionNode::connect(ProductionNode::Ptr node, BetaMemory::Ptr beta, Agenda::Ptr agenda)
{
    node->agenda_ = agenda;
    beta->addProduction(node);
}

} /* rete */
