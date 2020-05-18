#ifndef RETE_TRIPLECONSISTENCY_HPP_
#define RETE_TRIPLECONSISTENCY_HPP_

#include "../rete-core/AlphaNode.hpp"
#include "Triple.hpp"

namespace rete {

/**
    This AlphaNode is used to check if two fields of a triple are equal. E.g., if the condition is
    stated as "(?x self ?x)" we need to nodes: One for (predicate == "self"), and one for
    (subject == object), which is what this class provides.
*/
class TripleConsistency : public AlphaNode {
    Triple::Field field1_, field2_;

    std::string getDOTAttr() const override;
public:
    using Ptr = std::shared_ptr<TripleConsistency>;
    TripleConsistency(Triple::Field, Triple::Field);
    void activate(WME::Ptr, PropagationFlag) override;
    bool operator == (const AlphaNode& other) const override;

    std::string toString() const override;
};

} /* rete */


#endif /* end of include guard: RETE_TRIPLECONSISTENCY_HPP_ */
