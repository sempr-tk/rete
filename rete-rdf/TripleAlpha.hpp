#ifndef RETE_TRIPLEALPHA_HPP_
#define RETE_TRIPLEALPHA_HPP_

#include "../rete-core/AlphaNode.hpp"
#include "Triple.hpp"

namespace rete {

/**
    An AlphaNode specifically for testing Triple-WMEs. It can be a check on either subject,
    predicate, or object. Concatenate 3 of these to check all parts of a Triple.
*/
class TripleAlpha : public AlphaNode {
    Triple::Field field_;
    std::string value_;

    std::string getDOTAttr() const override;
public:
    TripleAlpha(Triple::Field field, const std::string& value);

    void activate(WME::Ptr, PropagationFlag) override;
    bool operator == (const AlphaNode& other) const override;

    std::string toString() const override;
};

} /* rete */

#endif /* end of include guard: RETE_TRIPLEALPHA_HPP_ */
