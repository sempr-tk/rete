#ifndef RETE_BACKEDWME_HPP_
#define RETE_BACKEDWME_HPP_

#include <set>

#include "../rete-core/WME.hpp"
#include "Evidence.hpp"
#include "EvidenceComparator.hpp"

namespace rete {

/**
    Adds evidences to a WME.
*/
class BackedWME {
    const WME::Ptr wme_;

    /**
        BackedWME is used in a set, where it is stored as const. But for the set only the WME matters, not the evidence
    */
    mutable std::set<Evidence::Ptr, EvidenceComparator> evidences_;
public:
    BackedWME(WME::Ptr);

    WME::Ptr getWME() const;

    /**
        Returns true if there are no evidences for the wrapped WME
    */
    bool isBacked() const;

    /**
        Adds an evidence. Compares by value, not the pointer
    */
    void addEvidence(Evidence::Ptr) const;

    /**
        Removes an evidence. Compares by value, not the pointer
    */
    void removeEvidence(Evidence::Ptr) const;

    /**
        Allow iteration over evidences
    */
    typedef decltype(evidences_.begin()) Iterator;
    Iterator begin() const;
    Iterator end() const;


    /**
        Compare two BackedWMEs by simply checking if the WME is the same
    */
    class SameWME {
    public:
        bool operator () (const BackedWME& a, const BackedWME& b) const;
    };
};


} /* rete */


#endif /* end of include guard: RETE_BACKEDWME_HPP_ */
