#ifndef RETE_AGENDA_HPP_
#define RETE_AGENDA_HPP_

#include <queue>
#include <vector>
#include <set>

#include "WME.hpp"
#include "Token.hpp"
#include "Production.hpp"

namespace rete {

typedef std::pair<Token::Ptr, Production::Ptr> AgendaItem;

/**
    Implements comparison of AgendaItems based on the priority of their Production
*/
class AgendaItemComparator {
public:
    bool operator () (const AgendaItem&, const AgendaItem&) const;
};


/**
    The Agenda holds all matches for production nodes that were not yet processed. When one WME is
    added to the network it can produce multiple matches for multiple production nodes, but instead
    of executing them as soon as a terminal node is found, a pair containing the token and the
    production node gets added to the agenda.
*/
class Agenda {
    // std::priority_queue<AgendaItem, std::vector<AgendaItem>, AgendaItemComparator> queue_;
    std::set<AgendaItem, AgendaItemComparator> queue_;
public:
    using Ptr = std::shared_ptr<Agenda>;

    /**
        Add another item to the agenda
    */
    void add(AgendaItem);
    void add(Token::Ptr, Production::Ptr);

    /**
        Remove an item from the agenda
    */
    void remove(AgendaItem);

    /**
        Check if the Agenda is empty
    */
    bool empty() const;

    /**
        Removes the first item from the agenda
    */
    void pop_front();

    /**
        Returns the first item on the agenda (by value)

        Calling front() on an empty agenda is undefined.
    */
    AgendaItem front() const;
};

} /* rete */

#endif /* end of include guard: RETE_AGENDA_HPP_ */
