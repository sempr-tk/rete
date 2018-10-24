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
    bool equals(const Accessor& other) const
    {
        auto o = dynamic_cast<TupleWMEAccessor*>(&other);
        if (o) return true; // everything encoded in the type -- WME-type,
        return false;
    }
public:
    T value(WME::Ptr wme) const override
    {
        auto twme = std::static_pointer_cast<TWME>(wme);
        return std::get<I>(twme->value_);
    }


};


} /* rete */


#endif /* end of include guard: RETE_TUPLEWMEACCESSOR_HPP_ */
