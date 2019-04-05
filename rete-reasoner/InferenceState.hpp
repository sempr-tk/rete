#ifndef RETE_REASONER_INFERENCE_STATE_HPP_
#define RETE_REASONER_INFERENCE_STATE_HPP_

#include <vector>
#include <map>
#include <set>

#include "../rete-core/WME.hpp"
#include "../rete-core/Token.hpp"

#include "BackedWME.hpp"
#include "EvidenceComparator.hpp"
#include "ExplanationVisitor.hpp"

namespace rete {


class Reasoner;

/**
    Contains one WME and all Evidences that support it
*/
struct WMESupportedBy {
    WME::Ptr wme_;
    std::vector<Evidence::Ptr> evidences_;
};

/**
    Contains one Evidence and all WMEs it supports
*/
struct EvidenceSupports {
    Evidence::Ptr evidence_;
    std::vector<WME::Ptr> wmes_;
};

/**
    The InferenceState keeps information about the current state of the inference graph -- which
    WME was inferred through which rules and tokens, etc.
    It can be copied to get a snapshot of the reasoners state. Be aware that it only contains
    pointers to the WMEs and Evidences involved. In most cases this should not be a problem, since
    the rete network requires the WMEs to be immutable either way.

    (Introduced to keep ExplanationIterators valid)
*/
class InferenceState {
    friend class Reasoner;

    /**
        The set of WMEs backed by some evidence
    */
    std::set<BackedWME, BackedWME::SameWME> backedWMEs_;

    /**
        Index to easily find all WMEs that are backed by a given evidence.
    */
    std::map<Evidence::Ptr, std::vector<WME::Ptr>, EvidenceComparator> evidenceToWME_;

    // list of WMEs already explained or queued for explanation
    mutable std::set<WME::Ptr> explained_;
    // helper methods for traverseExplanation
    void traverse(WME::Ptr, ExplanationVisitor&, int depth) const;
    void traverse(Evidence::Ptr, ExplanationVisitor&, int depth) const;
public:
    /**
        Returns all evidences that back a given WME.
    */
    WMESupportedBy explain(WME::Ptr) const;

    /**
        Returns all WMEs supported by this evidence
    */
    EvidenceSupports explainedBy(Evidence::Ptr) const;

    /**
        Traverse the WMEs and Evidences relevant for the explanation of the given WME, and apply
        the visitor.
    */
    void traverseExplanation(WME::Ptr toExplain, ExplanationVisitor& visitor) const;

    /** Returns the number of WMEs */
    size_t numWMEs() const;
    /** Returns the number of evidences */
    size_t numEvidences() const;

    /** Returns all WMEs */
    std::vector<WME::Ptr> getWMEs() const;
};

}

#endif /* end of include guard: RETE_REASONER_INFERENCE_STATE_HPP_ */
