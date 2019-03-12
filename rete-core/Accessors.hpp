#ifndef RETE_VALUEACCESSOR_HPP_
#define RETE_VALUEACCESSOR_HPP_

#include <string>
#include <memory>
#include <set>
#include <typeindex>

#include "WME.hpp"
#include "Token.hpp"
#include "Util.hpp"

namespace rete {

template <class Type, class Self> class ValueAccessor;

/**
    The Accessor abstracts the access to concrete values in WMEs. A concrete implementation
    could e.g. return the predicate of a rdf-triple, or one element of a std::tuple.

    This base class also manages a list of concrete types it implements, and therefore allows to
    call
        accessor->canAs<Type>()
    and
        accessor->as<Type>()
    to easily check for and use a concrete type.
    TODO: This could/should be done for WMEs, too(?)
*/
class Accessor {
    /**
        Implementation dependent equality check for Accessors.
    */
    virtual bool equals(const Accessor& other) const = 0;

protected:
    std::set<std::type_index> types_;
    int index_;

    template <class T>
    void registerType()
    {
        types_.insert(typeid(T));
    }

public:
    using Ptr = std::shared_ptr<Accessor>;

    /**
        Creates a new ValueAccessor. The index defines where in a given token to search. Index of
        -1 (default) specifies that this ValueAccessor may only be used with a WME, not with a
        token.
    */
    Accessor(int index = -1);
    virtual ~Accessor();

    int& index() { return index_; }
    int const& index() const { return index_; }

    /**
        returns true if the Accessor can be cast to Type
    */
    template <class Type>
    bool canAs() const
    {
        return types_.find(typeid(Type)) != types_.end();
    }

    /**
        static_casts this to a pointer to Type
    */
    template <class Type>
    Type* as()
    {
        return static_cast<Type*>(this);
    }

    /**
        Equality between accessors is given when they use the same token-index, are of the same type
        and access the same part of a WME.
        This operator only implements the token-index-check and calls the pure virtual equals-method
        which provides the implementation dependent checks.
    */
    bool operator == (const Accessor& other) const
    {
        if (this == &other) return true;
        if (this->index_ == other.index_) return this->equals(other);
        return false;
    }

    /**
        In order to do generic joins with any two Accessors, we need a way to compare them, or
        rather, the values that they point to given a token and a wme. This is difficult through a
        pointer to base, so instead we provide a virtual method that allows any Accessor to check
        if some other one implements the same ValueAccessor-interface as itself, cast it and do
        the check.
    */
    virtual bool canCompareValues(const Accessor& other) const = 0;
    virtual bool valuesEqual(Accessor& other, Token::Ptr token, WME::Ptr wme) = 0;

    /**
        Accessors must be clonable! In a list of variable bindings we will need to increment the
        index, but the nodes need access to the index they were given. (stupid reverse counting
        index, where 0 is the last element...)
    */
    virtual Accessor* clone() const = 0;

    /**
        For visualization purposes, every Accessor may implement a toString-method. This is used
        in nodes to show what variables they access. The base-implementation simply returns
        "Accessor(index)".
    */
    virtual std::string toString() const
    {
        return "Accessor(" + std::to_string(index_) + ")";
    }

};


/**
    A helper class to create a unified interface for all types of values.
    Simply adds a pure virtual getValue(WME::Ptr, Type&) method (which has to be overridden) and a
    helper getValue(Token::Ptr, Type&).

    Type: The value that can be accessed through accessors that implement this interface
    Self: The type that inherits the ValueAccessor. CRTP to access the index_ value from Accessor.
*/
template <class Type, class Self>
class ValueAccessor {
public:
    virtual ~ValueAccessor() {}

    /**
        Returns the value extracted from the WME as <Type>.
        Sadly this can't use the return type, or else you won't be able to implement multiple
        ValueAccessor<T>s in one accessor (return type is not part of the function signature).
    */
    virtual void getValue(WME::Ptr, Type& value) const = 0;

    /**
        Returns the value extracted from the WME in the token.
    */
    void getValue(Token::Ptr token, Type& value) const
    {
        if (static_cast<const Self*>(this)->index() < 0)
            throw std::invalid_argument("ValueAccessor constructed for WMEs only applied to Token");

        int count = static_cast<const Self*>(this)->index();
        while (count > 0 && token)
        {
            token = token->parent;
            count--;
        }

        if (!token)
            throw std::out_of_range(
                "ValueAccessor indexes entry no. " + std::to_string(static_cast<const Self*>(this)->index()) +
                " but the Token has " + std::to_string(count+1) + " entries too few.");

        this->getValue(token->wme, value);
    }

};

/**
    An Interface for Accessors that provide a string representation of their value
*/
class StringAccessor : public Accessor, public ValueAccessor<std::string, StringAccessor> {
protected:
    StringAccessor();
public:
    bool canCompareValues(const Accessor& other) const override;
    bool valuesEqual(Accessor& other, Token::Ptr token, WME::Ptr wme) override;

    // convenience
    std::string getString(WME::Ptr wme);
    std::string getString(Token::Ptr token);
};


/**
    An Interface for Accessors that just want to support one single type.
    Used as a default-case for the TupleWMEAccessors.
*/
template <class Type>
class SpecificTypeAccessor : public Accessor,
                             public ValueAccessor<Type, SpecificTypeAccessor<Type>> {
protected:
    SpecificTypeAccessor() { registerType<SpecificTypeAccessor>(); }
};


/**
    An Interface for Accessors that provide a number. Must also provide a string representation.
*/
class NumberAccessor : public StringAccessor,
                       public ValueAccessor<int, NumberAccessor>,
                       public ValueAccessor<long, NumberAccessor>,
                       public ValueAccessor<float, NumberAccessor>,
                       public ValueAccessor<double, NumberAccessor> {

protected:
    NumberAccessor();

public:
    virtual bool canGetInt() const = 0;
    virtual bool canGetLong() const = 0;
    virtual bool canGetFloat() const = 0;
    virtual bool canGetDouble() const = 0;

    using StringAccessor::getValue;
    using ValueAccessor<int, NumberAccessor>::getValue;
    using ValueAccessor<long, NumberAccessor>::getValue;
    using ValueAccessor<float, NumberAccessor>::getValue;
    using ValueAccessor<double, NumberAccessor>::getValue;

    // convenience
    int getInt(WME::Ptr wme) const { int tmp; getValue(wme, tmp); return tmp; }
    long getLong(WME::Ptr wme) const { long tmp; getValue(wme, tmp); return tmp; }
    float getFloat(WME::Ptr wme) const { float tmp; getValue(wme, tmp); return tmp; }
    double getDouble(WME::Ptr wme) const { double tmp; getValue(wme, tmp); return tmp; }
    int getInt(Token::Ptr token) const { int tmp; getValue(token, tmp); return tmp; }
    long getLong(Token::Ptr token) const { long tmp; getValue(token, tmp); return tmp; }
    float getFloat(Token::Ptr token) const { float tmp; getValue(token, tmp); return tmp; }
    double getDouble(Token::Ptr token) const { double tmp; getValue(token, tmp); return tmp; }

    bool canCompareValues(const Accessor& other) const override;
    bool valuesEqual(Accessor& other, Token::Ptr token, WME::Ptr wme) override;
};

/**
    A partial implementation of NumberAccessor, when the concrete type is already known.
*/
template <class T>
class SpecificNumAccessor : public NumberAccessor {
    /**
        Getter to be implemented by derived classes.
        Used to provide all pure virtual methods from the NumberAccessor interface.
    */
    virtual T internalValue(WME::Ptr) const = 0;
public:
    void getValue(WME::Ptr wme, int& value) const override { value = internalValue(wme); }
    void getValue(WME::Ptr wme, long& value) const override { value = internalValue(wme); }
    void getValue(WME::Ptr wme, float& value) const override { value = internalValue(wme); }
    void getValue(WME::Ptr wme, double& value) const override { value = internalValue(wme); }
    void getValue(WME::Ptr wme, std::string& value) const override
    {
        value = std::to_string(internalValue(wme));
    }

    // checks for the concrete number type
    bool canGetInt() const override { return util::safe_conversion<T, int>::value; }
    bool canGetLong() const override { return util::safe_conversion<T, long>::value; }
    bool canGetFloat() const override { return util::safe_conversion<T, float>::value; }
    bool canGetDouble() const override { return util::safe_conversion<T, double>::value; }
};



/**
    Accessors for constant values to simplify builtins.
*/
template <class T>
class ConstantNumberAccessor : public SpecificNumAccessor<T> {
    T value_;
    T internalValue(WME::Ptr) const override { return value_; }
public:
    ConstantNumberAccessor(const T& value) : value_(value) {}

    bool equals(const Accessor& other) const override
    {
        auto o = dynamic_cast<const ConstantNumberAccessor*>(&other);
        if (!o) return false;
        return value_ == o->value_;
    }

    ConstantNumberAccessor* clone() const override
    {
        return new ConstantNumberAccessor(*this);
    }
};

class ConstantStringAccessor : public StringAccessor {
    std::string value_;
public:
    ConstantStringAccessor(const std::string& value) : value_(value) {}
    void getValue(WME::Ptr, std::string& value) const override
    {
        value = value_;
    }

    bool equals(const Accessor& other) const override
    {
        auto o = dynamic_cast<const ConstantStringAccessor*>(&other);
        if (!o) return false;
        return value_ == o->value_;
    }

    ConstantStringAccessor* clone() const override
    {
        return new ConstantStringAccessor(*this);
    }

};

}


#endif /* end of include guard: RETE_VALUEACCESSOR_HPP_ */
