#ifndef RETE_TRIPLEACCESSOR_HPP_
#define RETE_TRIPLEACCESSOR_HPP_

#include "Triple.hpp"
#include "../rete-core/Accessors.hpp"

namespace rete {

/**
    Accessors for rdf triples

    TODO: This is rather quickly hacked together. It should not inherit SpecificNumAccessor<float>,
        but rather NumberAccessor, and implement all the checks and conversions itself, and base
        them on the actual string inside the triple. It needs to take typed literals into account,
        so e.g. "3.14"^^<xsd:float> is correctly parsed as a float.

    For now, it just assumes plain numbers without any extra annotations, quotations etc., and
    parses them with a stringstream.
*/
class TripleAccessor : public SpecificNumAccessor<float> {
    Triple::Field field_;
    bool equals(const Accessor& other) const override;
    float internalValue(WME::Ptr) const override;
public:
    TripleAccessor(Triple::Field field);
    void getValue(WME::Ptr, std::string& value) const override;
    std::string toString() const override;

    TripleAccessor* clone() const override;

    bool canCompareValues(const Accessor& other) const override;
    bool valuesEqual(Accessor& other, Token::Ptr token, WME::Ptr wme) override;
};


} /* rete */


#endif /* end of include guard: RETE_TRIPLEACCESSOR_HPP_ */
