#ifndef RETE_WMECOMPARATOR_HPP_
#define RETE_WMECOMPARATOR_HPP_

#include "WME.hpp"

namespace rete {

/**
    This class implements an ordering for shared_ptr to WMEs by simply comparing
    the pointed at WMEs.
*/
class WMEComparator {
public:
    bool operator () (const WME::Ptr& a, const WME::Ptr& b) const;
};

} /* rete */


#endif /* end of include guard: RETE_WMECOMPARATOR_HPP_ */
