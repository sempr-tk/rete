#include <vector>

#include "EvidenceComparator.hpp"


namespace rete {

bool EvidenceComparator::operator() (const Evidence::Ptr& a, const Evidence::Ptr& b) const
{
    if (a == b) return false;
    return *a < *b;
}

} /* rete */
