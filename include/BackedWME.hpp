#ifndef RETE_BACKEDWME_HPP_
#define RETE_BACKEDWME_HPP_

#include <vector>

#include "WME.hpp"
#include "Evidence.hpp"

namespace rete {

/**
    Adds evidences to a WME.
*/
class BackedWME {
    const WME::Ptr wme_;

    /**
        BackedWME is used in a set, where it is stored as const. But for the set only the WME matters, not the evidence
    */
    mutable std::vector<Evidence::Ptr> evidences_;
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
        Compare two BackedWMEs by simply checking if the WME is the same
    */
    class SameWME {
    public:
        bool operator () (const BackedWME& a, const BackedWME& b) const;
    };
};


} /* rete */


#endif /* end of include guard: RETE_BACKEDWME_HPP_ */
