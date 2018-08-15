#include "../include/WMEComparator.hpp"

#include <iostream>

namespace rete {

bool WMEComparator::operator() (const WME::Ptr& a, const WME::Ptr& b) const
{
    return *a < *b;
}

} /* rete */
