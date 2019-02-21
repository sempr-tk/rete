#ifndef RETE_VALUEACCESSOR_HPP_
#define RETE_VALUEACCESSOR_HPP_

#include <string>
#include <memory>
#include <set>
#include <typeindex>

#include "WME.hpp"
#include "Token.hpp"

namespace rete {

template <class Type> class ValueAccessor;

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
        returns true if the ValueAccessor can be cast to the typed accessor ValueAccessor<Type>
    */
    template <class Type>
    bool canAs() const
    {
        return types_.find(typeid(ValueAccessor<Type>)) != types_.end();
    }

    /**
        casts this to a pointer to ValueAccessor<Type>.
    */
    template <class Type>
    ValueAccessor<Type>* as()
    {
        return static_cast<ValueAccessor<Type>*>(this);
    }

    /**
        Shortcut to get the value. Use canAs<T> before trying this!
    */
    template <class Type>
    Type value(WME::Ptr wme)
    {
        Type tmp;
        this->as<Type>()->getValue(wme, tmp);
        return tmp;
    }

    template <class Type>
    Type value(Token::Ptr token)
    {
        Type tmp;
        this->as<Type>()->getValue(token, tmp);
        return tmp;
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
    The real interface of accessors is specific to the type they support. The ValueAccessor-class
    only declares virtual methods to be implemented by the actual accessor class
    (e.g. TripleAccessor).

    Nodes may want to check for ValueAccessor<std::string>, ValueAccessor<float> or any other
    ValueAccessor<Type>.
*/
template <class Type>
class ValueAccessor : public Accessor {
public:
    using Ptr = std::shared_ptr<ValueAccessor>;

    ValueAccessor() : Accessor()
    {
        // register the type
        types_.insert(typeid(ValueAccessor<Type>));
    }

    ValueAccessor(int index) : Accessor(index)
    {
        // register the type
        types_.insert(typeid(ValueAccessor<Type>));
    }

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
        if (index_ < 0)
            throw std::invalid_argument("ValueAccessor constructed for WMEs only applied to Token");

        int count = index_;
        while (count > 0 && token)
        {
            token = token->parent;
            count--;
        }

        if (!token)
            throw std::out_of_range(
                "ValueAccessor indexes entry no. " + std::to_string(index_) +
                " but the Token has " + std::to_string(count) + " entries too few.");

        this->getValue(token->wme, value);
    }

    /**
        Check if the other Accessor implements this type, too.
    */
    bool canCompareValues(const Accessor& other) const override
    {
        return other.canAs<Type>();
    }

    /**
        Compare the values accessed by this and the other accessor. Performs a static cast, so use
        "canCompareValues" before calling this method!
    */
    bool valuesEqual(Accessor& other, Token::Ptr token, WME::Ptr wme) override
    {
        ValueAccessor& o = static_cast<ValueAccessor&>(other);
        // handle the different cases, where both accessors can either be created to access a WME
        // or a token. Normally, this method should only be called by joins, where one is applied
        // to a token and the other to a wme.
        if      (index() == -1 && o.index() == -1)return value<Type>(wme)   == o.value<Type>(wme);
        else if (index() != -1 && o.index() == -1)return value<Type>(token) == o.value<Type>(wme);
        else if (index() == -1 && o.index() != -1)return value<Type>(wme)   == o.value<Type>(token);
             /* (index() != -1 && o.index() != -1) */
        else                                      return value<Type>(token) == o.value<Type>(token);
    }
};


/**
    Helper do define multiple supported types at once
    TODO: I guess this is broken due to ambiguous method declariations (some signature with different return types)? And the canCompareValues and valuesEqual methods must be a bit more sophisticated in this case...
*/
template <class... Types>
class ValueAccessors : public ValueAccessor<Types>... {};

}

#endif /* end of include guard: RETE_VALUEACCESSOR_HPP_ */
