#include "../include/AgendaNode.hpp"

namespace rete {

AgendaNode::AgendaNode(Production::Ptr p, Agenda::Ptr a)
    : ProductionNode(p), agenda_(a)
{
}


std::string AgendaNode::getDOTAttr() const
{
    return "[label=\"AgendaNode \'" + getName() + "\'\\n" + production_->getName() + "\"]";
}


void AgendaNode::activate(Token::Ptr token, PropagationFlag flag)
{
    if (flag == rete::ASSERT)
    {
        // if there has been a RETRACT for this token-production pair that has not yet been executed, assume that the effects of a previous ASSERT still hold and no RETRACT as to be executed by the production. So, silently remove it from the agenda without adding the ASSERT.
        // NOTE: Use removeEquivalent because the token is new (the one in the RETRACT-item we search for has obviously been retracted from the BetaMemory).
        bool removed = agenda_->removeEquivalent(AgendaItem{token, production_, rete::RETRACT});
        // only if no RETRACT was on the agenda we have to execute (and thus add to the agenda) an ASSERT
        if (!removed) agenda_->add(AgendaItem{token, production_, rete::ASSERT});

    } else {

        // the same argumentation as above:
        // if there was an ASSERT scheduled, just dont do it.
        // if there was no ASSERT scheduled, explicitely RETRACT.
        // NOTE: Use remove because we are retracting an existing token, not adding a new one
        bool removed = agenda_->remove(AgendaItem{token, production_, rete::ASSERT});
        if (!removed) agenda_->add(AgendaItem{token, production_, rete::RETRACT});
    }
}

} /* rete */
