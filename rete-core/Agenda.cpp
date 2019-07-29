#include <algorithm>
#include "Agenda.hpp"


namespace rete {

bool AgendaItemComparator::operator() (const AgendaItem& a, const AgendaItem& b) const
{
    // need a strict ordering, where !(a < b) && !(b < a) if equiv. to a == b
    // so a mere priority-comparison is not enough.
    int prioA = std::get<1>(a)->getPriority();
    int prioB = std::get<1>(b)->getPriority();

    // if they differ, just use them:
    if (prioA != prioB) return prioA > prioB;

    // special treatment if priorities are equal:
    // RETRACT before UPDATE before ASSERT:
    PropagationFlag flagA = std::get<2>(a);
    PropagationFlag flagB = std::get<2>(b);
    if (flagA != flagB)
    {
        switch(flagA)
        {
        case PropagationFlag::RETRACT:
            return true;
        case PropagationFlag::UPDATE:
            return flagB == PropagationFlag::ASSERT;
        case PropagationFlag::ASSERT:
            return false;
        }
    }

    // if tokens differ, use them for the ordering
    if (std::get<0>(a) != std::get<0>(b)) return std::get<0>(a) < std::get<0>(b); // just pointer comparison...
    // NOTE: For comparison of tokens I only use pointer-comparison here. The assumption is that they will be different -- else the rete network is malformed and there are two equivalent BetaNodes. For a clean Agenda it might be necessary to relax the token-comparision to by-value (with token->equals(other)). This is implemented in removeEquivalent(AgendaItem)

    // if tokens are the same, at least the production node might be different...
    if (std::get<1>(a) != std::get<1>(b)) return std::get<1>(a) < std::get<1>(b);
    // else... there is no difference. Pointer equality for both entries.
    return false;
}


void Agenda::add(AgendaItem item)
{
    queue_.insert(item);

    auto token = std::get<0>(item);
    auto production = std::get<1>(item);
    auto flag = std::get<2>(item);
    auto name = std::get<3>(item);

    bool removed;
    switch (flag)
    {
    case rete::ASSERT:
        // if there already is an assert on the agenda, nothing will happen (the queue is a set...)
        // if there already is an update on the agenda, something is going clearly wrong!
        // this should never happen, and the check could be removed I guess, but just to be
        // sure lets throw an exception...
        if (queue_.find(AgendaItem{token, production, rete::UPDATE, name}) != queue_.end())
        {
            throw std::exception();
        }

        // if there has been a RETRACT for this token-production pair that has not yet been executed, assume that the effects of a previous ASSERT still hold and no RETRACT has to be executed by the production. So, silently remove it from the agenda without adding the ASSERT.
        // NOTE: This changed recently: I removed "removeEquivalent", as it leads to incorrect evidence handling in the reasoner, who only looks at Token-identities (ptr address), not their content (which would be expensive). Hence, not retracting mtach (A) and not adding match (B) is not a good idea, even if their content is the same. There might be better solutions, e.g. some efficient way to treat tokens as defined by the values in their wmes, but this is something to be implemented later, if there is a need for it. And the time. Right now I mainly see the bug that needs be fixed, which is more important than the performance optimization.
        //  -- Also, the same (identical) token should never be asserted twice, so I could leave out this check, but leave it here to be sure to not have created some weird behaviour.
        removed = remove(AgendaItem{token, production, rete::RETRACT, name});

        if (removed) throw std::runtime_error("Agenda-Error: How could this happen? Tried to add ASSERT to the agenda for a match that is to be RETRACTED!");

        // just add the ASSERT
        queue_.insert(item);
        break;
    case rete::RETRACT:
        // the same argumentation as above:
        // if there was an ASSERT scheduled, just dont do it.
        // if there was no ASSERT scheduled, explicitely RETRACT.
        // NOTE: Use remove because we are retracting an existing token, not adding a new one
        removed = remove(AgendaItem{token, production, rete::ASSERT, name});
        if (!removed)
        {
            // also, remove any already present UPDATEs
            remove(AgendaItem{token, production, rete::UPDATE, name});
            // and insert the retract.
            queue_.insert(item);
        }
        break;
    case rete::UPDATE:
        // if there is an ASSERT for this token+production still on the agenda the update is unneccessary.
        if (queue_.find(AgendaItem{token, production, rete::ASSERT, name}) != queue_.end())
        {
            // so, no need to add the "update"
        }
        else
        {
            // is there a retract? why should there? if there is something is clearly wrong!
            // just for development/debugging make sure there isnt.
            if (queue_.find(AgendaItem{token, production, rete::RETRACT, name}) != queue_.end())
            {
                throw std::exception();
            }
            queue_.insert(item);
        }
    }
}

bool Agenda::remove(AgendaItem item)
{
    return (queue_.erase(item) > 0);
}

bool Agenda::empty() const
{
    return queue_.empty();
}

void Agenda::pop_front()
{
    queue_.erase(queue_.begin());
}

AgendaItem Agenda::front() const
{
    return *(queue_.begin());
}

} /* rete */
