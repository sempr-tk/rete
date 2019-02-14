#ifndef RETE_TRIPLEACCESSOR_HPP_
#define RETE_TRIPLEACCESSOR_HPP_

#include "Triple.hpp"
#include <rete-core/ValueAccessor.hpp>

namespace rete {

/**
    Accessors for rdf triples
*/
class TripleAccessor : public ValueAccessor<std::string> {
    Triple::Field field_;
    bool equals(const Accessor& other) const override;
public:
    TripleAccessor(Triple::Field field);
    std::string value(WME::Ptr) const override;
    std::string toString() const override;

    TripleAccessor* clone() const override;
};


} /* rete */


#endif /* end of include guard: RETE_TRIPLEACCESSOR_HPP_ */
