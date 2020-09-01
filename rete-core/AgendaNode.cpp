#include "AgendaNode.hpp"
#include "Util.hpp"

namespace rete {

AgendaNode::AgendaNode(Production::Ptr p, Agenda::Ptr a)
    : ProductionNode(p), agenda_(a)
{
}


std::string AgendaNode::getDOTAttr() const
{
    return "[label=\"AgendaNode \'" +
                util::dotEscape(getName()) + "\'\\n" +
                util::dotEscape(production_->toString()) + "\"]";
}

std::string AgendaNode::toString() const
{
    return "AgendaNode \'" + getName() + "\'\n" + production_->toString();
}

void AgendaNode::activate(Token::Ptr token, PropagationFlag flag)
{
    // just add it to the agenda. All the special cases on when to really add it, remove other items etc, are all handled inside Agenda::add(AgendaItem)
    agenda_->add(AgendaItem{token, production_, flag, getName()});
}

} /* rete */
