#ifndef RETE_SKOLEMACCESSOR_HPP_
#define RETE_SKOLEMACCESSOR_HPP_

#include "Skolem.hpp"
#include "TriplePart.hpp"
#include "../rete-core/Accessors.hpp"

namespace rete {

/**
    Accessor to Skolem. A bit special, because I want it to be
    interpretable as a TriplePart and as a string.
*/
class SkolemAccessor : public Accessor<Skolem, TriplePart, std::string> {
public:
    void getValue(Skolem::Ptr, TriplePart& value) const override;
    void getValue(Skolem::Ptr, std::string& value) const override;

    std::string toString() const override;
    SkolemAccessor* clone() const override;

    bool equals(const AccessorBase& other) const override;
};

}

#endif /* include guard: RETE_SKOLEMACCESSOR_HPP_ */
