#ifndef RETE_DEFS_HPP_
#define RETE_DEFS_HPP_

namespace rete {

/**
    Definitions of which I didn't know where else to put them.
*/

/**
    Flags to specify if a WME or Token is asserted or retracted from the knowledge base.
*/
enum PropagationFlag {
    ASSERT,
    RETRACT
};

} /* rete */


#endif /* end of include guard: RETE_DEFS_HPP_ */
