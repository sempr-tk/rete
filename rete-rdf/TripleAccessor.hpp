#ifndef RETE_TRIPLEACCESSOR_HPP_
#define RETE_TRIPLEACCESSOR_HPP_

#include "Triple.hpp"
#include "../rete-core/Accessors.hpp"

namespace rete {

/**
    Accessors for rdf triples
    TODO: Allow interpretation of values as numbers? --> Derive from NumberAccessor
*/
class TripleAccessor : public StringAccessor {
    Triple::Field field_;
    bool equals(const Accessor& other) const override;
public:
    TripleAccessor(Triple::Field field);
    void getValue(WME::Ptr, std::string& value) const override;
    std::string toString() const override;

    TripleAccessor* clone() const override;
};


} /* rete */


#endif /* end of include guard: RETE_TRIPLEACCESSOR_HPP_ */
