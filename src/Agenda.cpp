#include <algorithm>
#include "../include/Agenda.hpp"


namespace rete {

bool AgendaItemComparator::operator() (const AgendaItem& a, const AgendaItem& b) const
{
    // need a strict ordering, where !(a < b) && !(b < a) if equiv. to a == b
    // so a mere priority-comparison is not enough.
    int prioA = a.second->getPriority();
    int prioB = b.second->getPriority();

    // if they differ, just use them:
    if (prioA != prioB) return prioA > prioB;

    // special treatment if priorities are equal:
    // if tokens differ, use them for the ordering
    if (a.first != b.first) return a.first < b.first; // just pointer comparison...
    // if tokens are the same, at least the production node might be different...
    if (a.second != b.second) return a.second < b.second;
    // else... there is no difference. Pointer equality for both entries.
    return false;
}


void Agenda::add(AgendaItem item)
{
    queue_.insert(item);
}

void Agenda::add(Token::Ptr token, Production::Ptr node)
{
    queue_.insert({token, node});
}

void Agenda::remove(AgendaItem item)
{
    queue_.erase(item);
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
