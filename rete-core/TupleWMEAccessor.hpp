#ifndef RETE_TUPLEWMEACCESSOR_HPP_
#define RETE_TUPLEWMEACCESSOR_HPP_

#include <type_traits>
#include "Accessors.hpp"
#include "Util.hpp"

namespace rete {

/**
    Accessor for TupleWMEs. Be aware that the accessor does not check if the given WME is actually
    a TupleWME of the correct type. This is assumed to be the case, please make sure to keep track
    of the structure of your network and tokens.
*/
template <size_t I, typename TWME, typename T = typename util::extract_type<TWME, I>::type>
class TupleWMEAccessor : public Accessor<TWME, T> {
    bool equals(const AccessorBase& other) const override
    {
        auto o = dynamic_cast<const TupleWMEAccessor*>(&other);
        return o != nullptr; // everything encoded in the type -- WME-type,
    }
public:
    void getValue(std::shared_ptr<TWME> wme, T& value) const override
    {
        value = std::get<I>(wme->value_);
    }

    TupleWMEAccessor* clone() const override { return new TupleWMEAccessor(*this); } // TODO! shouldn't work due to deleted copy-ctor in AccessorBase
};


/**
    Helper to reduce redundant code for number types
*/
template <size_t I, typename TWME, typename T>
class TNumWMEAccessor : public Accessor<TWME, T, std::string> {
    bool equals(const AccessorBase& other) const override
    {
        auto o = dynamic_cast<const TNumWMEAccessor*>(&other);
        return o != nullptr;
    }
public:
    void getValue(std::shared_ptr<TWME> wme, T& value) const override
    {
        value = std::get<I>(wme->value_);
    }

    void getValue(std::shared_ptr<TWME> wme, std::string& value) const override
    {
        T v;
        getValue(wme, v);
        value = std::to_string(v);
    }
};

/**
    Specializations for number types
*/
template <size_t I, typename TWME>
class TupleWMEAccessor<I, TWME, int> : public TNumWMEAccessor<I, TWME, int> {
public:
    TupleWMEAccessor* clone() const override { return new TupleWMEAccessor(*this); }
};

template <size_t I, typename TWME>
class TupleWMEAccessor<I, TWME, long> : public TNumWMEAccessor<I, TWME, long> {
public:
    TupleWMEAccessor* clone() const override { return new TupleWMEAccessor(*this); }
};

template <size_t I, typename TWME>
class TupleWMEAccessor<I, TWME, float> : public TNumWMEAccessor<I, TWME, float> {
public:
    TupleWMEAccessor* clone() const override { return new TupleWMEAccessor(*this); }
};

template <size_t I, typename TWME>
class TupleWMEAccessor<I, TWME, double> : public TNumWMEAccessor<I, TWME, double> {
public:
    TupleWMEAccessor* clone() const override { return new TupleWMEAccessor(*this); }
};

} /* rete */


#endif /* end of include guard: RETE_TUPLEWMEACCESSOR_HPP_ */
