#ifndef RETE_TUPLEWMEACCESSOR_HPP_
#define RETE_TUPLEWMEACCESSOR_HPP_

#include <type_traits>
#include "ValueAccessor.hpp"
#include "Util.hpp"

namespace rete {

/**
    Accessor for TupleWMEs. Be aware that the accessor does not check if the given WME is actually
    a TupleWME of the correct type. This is assumed to be the case, please make sure to keep track
    of the structure of your network and tokens.
*/
template <size_t I, typename TWME, typename T = typename util::extract_type<TWME, I>::type>
class TupleWMEAccessor : public ValueAccessor<T> {
    bool equals(const Accessor& other) const override
    {
        auto o = dynamic_cast<const TupleWMEAccessor*>(&other);
        if (o) return true; // everything encoded in the type -- WME-type,
        return false;
    }
public:
    void getValue(WME::Ptr wme, T& value) const override
    {
        auto twme = std::static_pointer_cast<TWME>(wme);
        value = std::get<I>(twme->value_);
    }

    TupleWMEAccessor* clone() const override { return new TupleWMEAccessor(*this); }

};


} /* rete */


#endif /* end of include guard: RETE_TUPLEWMEACCESSOR_HPP_ */
