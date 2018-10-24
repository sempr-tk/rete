#ifndef RETE_TUPLEWME_HPP_
#define RETE_TUPLEWME_HPP_

#include "WME.hpp"
#include "TupleWMEAccessor.hpp"
#include <tuple>

namespace rete {

/**
    The TupleWME is a very basic WME implementation to carry arbitrary tuples.
*/
template <class... Types>
class TupleWME : public WME {
public:
    template <size_t I> using Accessor = TupleWMEAccessor<I, TupleWME>;
    template <size_t, typename, typename> friend class TupleWMEAccessor;
private:
    std::tuple<Types...> value_;
public:
    using Ptr = std::shared_ptr<TupleWME>;

    TupleWME(Types... args)
        : value_(args...)
    {
    }

    std::string toString() const override
    {
        return "TupleWME[" + std::to_string(sizeof...(Types)) + "]";
    }

    bool operator < (const WME& other) const override
    {
        auto o = dynamic_cast<const TupleWME*>(&other);
        if (o)
        {
            return value_ == o->value_;
        }
        return true; // should not happen
    }
};


} /* rete */


#endif /* end of include guard: RETE_TUPLEWME_HPP_ */
