#ifndef RETE_EVIDENCECOMPARATOR_HPP_
#define RETE_EVIDENCECOMPARATOR_HPP_

#include "Evidence.hpp"

namespace rete {

/**
    To keep Evidence::Ptr in a mep we need a comparator that compares the actual instances
*/
class EvidenceComparator {
public:
    bool operator () (const Evidence::Ptr& a, const Evidence::Ptr& b);
};

} /* rete */

#endif /* end of include guard: RETE_EVIDENCECOMPARATOR_HATORP_ */
