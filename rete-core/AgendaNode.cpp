#include "AgendaNode.hpp"

namespace rete {

AgendaNode::AgendaNode(Production::Ptr p, Agenda::Ptr a)
    : ProductionNode(p), agenda_(a)
{
}


std::string AgendaNode::getDOTAttr() const
{
    return "[label=\"AgendaNode \'" + getName() + "\'\\n" + production_->toString() + "\"]";
}


void AgendaNode::activate(Token::Ptr token, PropagationFlag flag)
{
    if (flag == rete::ASSERT)
    {
        // if there has been a RETRACT for this token-production pair that has not yet been executed, assume that the effects of a previous ASSERT still hold and no RETRACT has to be executed by the production. So, silently remove it from the agenda without adding the ASSERT.
        // NOTE: This changed recently: I removed "removeEquivalent", as it leads to incorrect evidence handling in the reasoner, who only looks at Token-identities (ptr address), not their content (which would be expensive). Hence, not retracting mtach (A) and not adding match (B) is not a good idea, even if their content is the same. There might be better solutions, e.g. some efficient way to treat tokens as defined by the values in their wmes, but this is something to be implemented later, if there is a need for it. And the time. Right now I mainly see the bug that needs be fixed, which is more important than the performance optimization.
        //  -- Also, the same (identical) token should never be asserted twice, so I could leave out this check, but leave it here to be sure to not have created some weird behaviour.
        bool removed = agenda_->remove(AgendaItem{token, production_, rete::RETRACT, getName()});
        if (removed) throw std::runtime_error("AgendaNode error: How could this happen? ASSERT for a token+production for which there already was a RETRACT?!");

        // just add the assert to the agenda.
        agenda_->add(AgendaItem{token, production_, rete::ASSERT, getName()});

    } else {

        // the same argumentation as above:
        // if there was an ASSERT scheduled, just dont do it.
        // if there was no ASSERT scheduled, explicitely RETRACT.
        // NOTE: Use remove because we are retracting an existing token, not adding a new one
        bool removed = agenda_->remove(AgendaItem{token, production_, rete::ASSERT, getName()});
        if (!removed) agenda_->add(AgendaItem{token, production_, rete::RETRACT, getName()});
    }
}

} /* rete */
