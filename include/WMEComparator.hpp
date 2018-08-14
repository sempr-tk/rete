#ifndef RETE_WMECOMPARATOR_HPP_
#define RETE_WMECOMPARATOR_HPP_

#include "WME.hpp"

namespace rete {

/**
    This class simply implements a Comparator for WMEs for a simple "ordering" in sets and vectors.
    Since the actual ordering does not matter but only the equality is needed, this is what it does:
        For a and b WMEs,
            if (a == b) return false;
            return &a < &b;
    If they are equal one is not less the other, and if not equal we just sort by memory address.
*/
class WMEComparator {
public:
    bool operator () (const WME::Ptr& a, const WME::Ptr& b) const;
};

} /* rete */


#endif /* end of include guard: RETE_WMECOMPARATOR_HPP_ */
