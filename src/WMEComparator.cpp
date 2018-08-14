#include "../include/WMEComparator.hpp"

#include <iostream>

namespace rete {

bool WMEComparator::operator() (const WME::Ptr& a, const WME::Ptr& b) const
{
    // pointer equality
    if (a == b) return false;

    // nullptr is less than everything
    if (!a) return true;
    if (!b) return false;

    // actual wme comparison
    if (*a == *b) return false;

    // ordering according to memory address, only equality is interesting.
    return (&a < &b);
}

} /* rete */
