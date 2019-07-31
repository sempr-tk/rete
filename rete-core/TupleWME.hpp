#ifndef RETE_TUPLEWME_HPP_
#define RETE_TUPLEWME_HPP_

#include "WME.hpp"
#include "TupleWMEAccessor.hpp"
#include "Util.hpp"
#include <tuple>

#include <sstream>

namespace rete {

/**
    The TupleWME is a very basic WME implementation to carry arbitrary tuples.
*/
template <class... Types>
class TupleWME : public WME {
    static const std::string type_;

public:
    std::tuple<Types...> value_;

    template <size_t I> using Accessor = TupleWMEAccessor<I, TupleWME>;
    template <size_t, typename, typename> friend class TupleWMEAccessor;

    using Ptr = std::shared_ptr<TupleWME>;

    TupleWME(Types... args)
        : value_(args...)
    {
    }

    std::string toString() const override
    {
        std::stringstream ss;
        ss << "TupleWME(";
        util::tuple_printer<decltype(value_)>::print(value_, ss);
        ss << ")";
        return ss.str();
        // return "TupleWME[" + std::to_string(sizeof...(Types)) + "]";
    }

    const std::string& type() const override
    {
        return TupleWME::type_;
    }

    bool operator < (const WME& other) const override
    {
        // for different types
        if (type() != other.type()) return type() < other.type();

        // for same type
        auto o = dynamic_cast<const TupleWME*>(&other);
        if (o)
        {
            return value_ < o->value_;
        }
        throw std::exception(); // should not happen
    }
};

// use typeid to create a unique name for the tuple type
template <class... Types> const std::string TupleWME<Types...>::type_(typeid(TupleWME<Types...>).name());

typedef TupleWME<> EmptyWME;

} /* rete */


#endif /* end of include guard: RETE_TUPLEWME_HPP_ */
