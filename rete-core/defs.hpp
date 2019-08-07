#ifndef RETE_DEFS_HPP_
#define RETE_DEFS_HPP_

namespace rete {

/**
    Definitions of which I didn't know where else to put them.
*/

/**
    Flags to specify what is currently being done to the wme or token that is passed through the
    network.
    ASSERT  - The WME/Token is added to the network, it is new data/ a new (sub-)match
    RETRACT - A WME or a Token in which the WME is present has to be removed
    UPDATE  - A WME has changed internally, and all matches in which it is present need to be
              re-evaluated. If the check of one node fails it propagates RETRACT instead of UPDATE,
              if everything still holds the UPDATE is propagated even into the agenda.
              Note: A Node might get triggert with an update for a token that previously didn't
              match, but creates new match(es, in case of a join) now. The node can just pass it as
              UPDATE to its memory, which has to check if the match was already there, and replace
              the flag if ASSERT if not.
              Processing a <UPDATE, Token, Production> assumes that the result might change, and
              thus first retracts the <Token, Production>-evidence, and afterwards activates the
              Production, possibly ASSERTing the old result again.
*/
enum PropagationFlag {
    ASSERT,
    RETRACT,
    UPDATE
};

} /* rete */


#endif /* end of include guard: RETE_DEFS_HPP_ */
