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
class TupleWMEAccessor : public SpecificTypeAccessor<T> {
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

/**
    Specializations for number and string types
*/
template <size_t I, typename TWME>
class TupleWMEAccessor<I, TWME, std::string> : public StringAccessor {
    bool equals(const Accessor& other) const override
    {
        auto o = dynamic_cast<const TupleWMEAccessor*>(&other);
        if (o) return true;
        return false;
    }
public:
    void getValue(WME::Ptr wme, std::string& value) const override
    {
        auto twme = std::static_pointer_cast<TWME>(wme);
        value = std::get<I>(twme->value_);
    }

    TupleWMEAccessor* clone() const override { return new TupleWMEAccessor(*this); }
};


/**
    Helper to reduce redundant code for number types
*/
template <size_t I, typename TWME, typename T>
class TNumWMEAccessor : public SpecificNumAccessor<T> {
    bool equals(const Accessor& other) const override
    {
        auto o = dynamic_cast<const TNumWMEAccessor*>(&other);
        if (o) return true;
        return false;
    }
public:
    T internalValue(WME::Ptr wme) const override
    {
        auto twme = std::static_pointer_cast<TWME>(wme);
        return std::get<I>(twme->value_);
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
