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
    // RETRACT before ASSERT:
    PropagationFlag flagA = std::get<2>(a);
    PropagationFlag flagB = std::get<2>(b);
    if (flagA != flagB) return (flagA == PropagationFlag::RETRACT ? true : false);

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
