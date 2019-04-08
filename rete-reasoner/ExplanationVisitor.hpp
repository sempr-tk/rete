#ifndef RETE_REASONER_EXPLANATION_VISITOR_HPP_
#define RETE_REASONER_EXPLANATION_VISITOR_HPP_

#include "../rete-core/WME.hpp"
#include "Evidence.hpp"
#include "AssertedEvidence.hpp"
#include "InferredEvidence.hpp"

namespace rete {

class Reasoner;
struct WMESupportedBy;

/**
    Abstract base class for any processing of explanations for a WME.
    Use this with the traverseExplanation method of the InferenceState. The visitor gets only very
    small pieces of information to chew on, namely instances of InferenceState::WMESupportedBy,
    which contain one WME and the Evidences it is suppored by. An evidence itself might rely on
    other WMEs, but which of them are to be traversed (explained) further it up the implementation
    of traverseExplanation.
*/
class ExplanationVisitor {
public:
    virtual ~ExplanationVisitor() {}
    /** Process information about which evidences support a WME */
    virtual void visit(WMESupportedBy&, size_t depth) = 0;

    /** Process WMEs */
    virtual void visit(WME::Ptr, size_t depth) = 0;

    /** Process Evidences */
    virtual void visit(Evidence::Ptr, size_t depth) = 0;
    virtual void visit(AssertedEvidence::Ptr, size_t depth) = 0;
    virtual void visit(InferredEvidence::Ptr, size_t depth) = 0;
};

} /* rete */

#endif /* end of include guard: RETE_REASONER_EXPLANATION_VISITOR_HPP_ */
