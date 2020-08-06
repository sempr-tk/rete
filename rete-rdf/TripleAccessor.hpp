#ifndef RETE_TRIPLEACCESSOR_HPP_
#define RETE_TRIPLEACCESSOR_HPP_

#include "Triple.hpp"
#include "TriplePart.hpp"
#include "../rete-core/Accessors.hpp"

namespace rete {

/**
    Accessors for rdf triples

    TODO: This is rather quickly hacked together.
        It needs to take typed literals into account,
        so e.g. "3.14"^^<xsd:float> is correctly parsed as a float.

    For now, it just assumes plain numbers without any extra annotations, quotations etc., and
    parses them with a stringstream.
*/
class TripleAccessor : public Accessor<Triple, TriplePart, std::string, float> {
    Triple::Field field_;
public:
    TripleAccessor(Triple::Field field);
    void getValue(Triple::Ptr, std::string& value) const override;
    void getValue(Triple::Ptr, float& value) const override;
    void getValue(Triple::Ptr, TriplePart& value) const override;

    std::string toString() const override;

    TripleAccessor* clone() const override;

    bool equals(const AccessorBase& other) const override;
};


} /* rete */


#endif /* end of include guard: RETE_TRIPLEACCESSOR_HPP_ */
