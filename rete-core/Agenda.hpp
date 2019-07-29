#ifndef RETE_AGENDA_HPP_
#define RETE_AGENDA_HPP_

#include <queue>
#include <vector>
#include <set>
#include <utility>
#include <string>


#include "WME.hpp"
#include "Token.hpp"
#include "Production.hpp"
#include "defs.hpp"


namespace rete {

// a AgendaItem:
// token (data fulfilling conditions), production (what to do), flag (assert/retract), string (name of the rule that triggered this production, just cosmetics)
typedef std::tuple<Token::Ptr, Production::Ptr, PropagationFlag, std::string> AgendaItem;

/**
    Implements comparison of AgendaItems based on the priority of their Production
*/
class AgendaItemComparator {
public:
    bool operator () (const AgendaItem&, const AgendaItem&) const;
};


/**
    The Agenda holds all matches for production nodes that were not yet processed, as well as
    retracted matches. When one WME is added to the network it can produce multiple matches for
    multiple production nodes, but instead of executing them as soon as a terminal node is found, a
    tuple containing the token and the production node gets added to the agenda.

    When a token is retracted that lead to a match of a production, it might be necessary to act on
    that, too, so in that case the same tuple of the token and the production is added to the
    agenda, but with a removal-parameter as the third entry (PropagationFlag::RETRACT instead of
    PropagationFlag::ASSERT).

    A difficult addition is the introduction of mutable WMEs. For these we need UPDATE-flags
    whenever their internal values change. The productions of a match that includes a changed WME
    have to be re-run even if the whole conditions/match still holds, since the result might be
    different. So we also need to add AgendaItems with UPDATE flags, and have to think about what to
    do when adding a new AgendaItem for an existing <Token, Production> combination with a different
    flag.
    [already there] + [new to add] = [what to do]
    ASSERT  + RETRACT = remove both from agenda
    RETRACT + ASSERT  = cannot happen, new match will be a new token (same content, still different)
    ASSERT  + UPDATE  = UPDATE unnecessary, noone was informed of the existence of the match yet
    RETRACT + UPDATE  = cannot happen, no way to "UPDATE" a match that was retracted from the net
    UPDATE  + ASSERT  = cannot happen, why should the match be ASSERTED again when it was even updated before?
    UPDATE  + RETRACT = UPDATE needs to be removed from the agenda, RETRACT added.

*/
class Agenda {
    std::set<AgendaItem, AgendaItemComparator> queue_;
public:
    using Ptr = std::shared_ptr<Agenda>;

    /**
        Add another item to the agenda
    */
    void add(AgendaItem);

    /**
        Remove an item from the agenda. Returns true if the AgendaItem was found and removed.
        This method requires the AgendaItem to match exactly, including the Token::Ptr.
    */
    bool remove(AgendaItem);


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
