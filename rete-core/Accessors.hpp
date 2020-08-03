#ifndef RETE_VALUEACCESSOR_HPP_
#define RETE_VALUEACCESSOR_HPP_

#include <string>
#include <memory>
#include <set>
#include <vector>
#include <typeindex>
#include <functional>
#include <cassert>
#include <algorithm>

#include "WME.hpp"
#include "Token.hpp"
#include "Util.hpp"

namespace rete {

/**
    The InterpretationBase is the base class for the implementation of
    different interpretations of a value accessor. It provides an interface
    that allows you to compare if the values accessed by this InterpretationBase
    and another InterpretationBase in a given Token/WME are equal.

    NOTE: Be careful when using it. The valuesEqual method *assumes* that both
    Interpretations are of the *same type*, even though only InterpretationBase&
    is used here! This is mainly for the GenericJoin node which takes pairs
    of InterpretationBase* to do the check on the data.
*/
class InterpretationBase {
public:
    virtual ~InterpretationBase() = default;

    /**
        Casts other to the same Interpretation<T> as this (statically!) and
        checks if the values returned by this and other applied to the given
        token/wme are equal.
    */
    virtual bool valuesEqual(const InterpretationBase& other,
                             Token::Ptr token,
                             WME::Ptr wme) const = 0;
};


// forward declaration of Interpretation<T>
template <class T> class Interpretation;

// ... and of PersistentInterpretation<T>
template <class T> struct PersistentInterpretation;

/**
    Objects of type AccessorBase are used to grant access to values inside of
    WMEs. To support a very generic functionality inside the reasoner / pattern
    matching it removes the dependency on the concrete WME type. A subclass of
    AccessorBase implements functions to get exactly one part, one value of a
    WME, e.g. the "age" member of a "Person" WME. If the member can be casted
    or converted to different value types, e.g. the age can be interpreted as
    an integer or a string, the AccessorBase-Subclass can register these
    Interpretation<T>s at the AccessorBase. The users of AccessorBase* can then
    ask it for these interpretations and use them to retrieve the data in a
    format they like (and is supported by the accessor), without knowing the
    type of WME they are accessing or the type of the Accessor they are using.

    The AccessorBase contains a vector of {type_index, InterpretationBase*} --
    mappings of specific types to the interpretation instances which allow us
    to retrieve that type from a Token/WME. The vector is filled by subclasses
    of AccessorBase.
*/
class AccessorBase {
    /**
        Implementation dependent equality check for Accessors.
        Necessary to check if two join nodes are equal.
    */
    virtual bool equals(const AccessorBase& other) const = 0;

protected:
    int index_;

    typedef std::pair<std::type_index, InterpretationBase*> TypeInterpretationPair;
    /// A vector containing all possible interpretations of the accessed value
    std::vector<TypeInterpretationPair> interpretations_;

public:
    using Ptr = std::shared_ptr<AccessorBase>;

    /**
        Creates a new Accessor. The index defines where in a given token to
        search. Index of -1 (default) specifies that this Accessor may only be
        used with a WME, not with a token.
    */
    AccessorBase(int index = -1);

    // delete copy and move ctors for now, as this is a bit difficult with
    // the function bindings in the interpretations
    AccessorBase(const AccessorBase&) = delete;
    AccessorBase(AccessorBase&&) = delete;
    AccessorBase& operator = (const AccessorBase&) = delete;
    AccessorBase& operator = (AccessorBase&&) = delete;

    virtual ~AccessorBase();

    int& index();
    int const& index() const;


    /**
        Returns a pair of interpretations that return the same value type,
        where the first is from 'this' and the second is from 'other'.
        Returns a pair of nullptr if no common interpretation is found.
    */
    std::pair<InterpretationBase*, InterpretationBase*>
        getCommonInterpretation(const AccessorBase& other) const;

    /**
        Returns all common interpretations of 'this' and 'other'.
    */
    std::vector<std::pair<InterpretationBase*, InterpretationBase*>>
        getCommonInterpretations(const AccessorBase& other) const;

    /**
        Returns an Interpretation through which you can get a value of T from
        a Token/WME, or a nullptr if no such interpretation exists for this
        accessor. The returned pointer is owned by the accessor.
    */
    template <class T>
    const Interpretation<T>* getInterpretation() const
    {
        auto it = std::find_if(
                    interpretations_.begin(),
                    interpretations_.end(),
                    [](const TypeInterpretationPair& pair)
                    {
                        return pair.first == typeid(T);
                    });

        if (it == interpretations_.end())
        {
            return nullptr;
        }
        else
        {
            return static_cast<Interpretation<T>*>(it->second);
        }
    }


    /**
        Equality between accessors is given when they use the same token-index,
        are of the same type and access the same part of a WME.
        This operator only implements the token-index-check and calls the pure
        virtual equals-method which provides the implementation dependent
        checks.
    */
    bool operator == (const AccessorBase& other) const
    {
        if (this == &other)
            return true;
        else if (this->index_ == other.index_)
            return this->equals(other);
        else
            return false;
    }

    /**
        Accessors must be clonable! In a list of variable bindings we will need
        to increment the index, but the nodes need access to the index they
        were given. (Remember the reverse-counting index, where 0 is always the
        newest/last element)
    */
    virtual AccessorBase* clone() const = 0;

    /**
        For visualization purposes, every Accessor may implement a toString-method. This is used
        in nodes to show what variables they access. The base-implementation simply returns
        "Accessor(index)".
    */
    virtual std::string toString() const;

};


/**
    The PersistentInterpretation is a utility struct: It wraps an
    Interpretation<T> together with the accessor it is a part of and which keeps
    the Interpretation<T> alive and valid.
    (There are std::bind(...)s to the accessor stored i n the Interpretation!)
*/
template <class T>
struct PersistentInterpretation {
private:
    AccessorBase* accessor_;
    const Interpretation<T>* interpretation_;
public:
    AccessorBase* const& accessor = accessor_;
    const Interpretation<T>* const& interpretation = interpretation_;


    PersistentInterpretation(const PersistentInterpretation&) = delete;
    PersistentInterpretation& operator = (const PersistentInterpretation&) = delete;

    PersistentInterpretation()
        : accessor_(nullptr), interpretation_(nullptr)
    {
    }

    /**
        Takes ownership of the given AccessorBase
    */
    PersistentInterpretation(AccessorBase* a)
        : accessor_(a), interpretation_(a->getInterpretation<T>())
    {
    }

    PersistentInterpretation(PersistentInterpretation&& other)
    {
        // just swap values. If this held actual data before, the dtor of other
        // will take care of it.
        std::swap(accessor_, other.accessor_);
        std::swap(interpretation_, other.interpretation_);
    }

    PersistentInterpretation& operator = (PersistentInterpretation&& other)
    {
        std::swap(accessor_, other.accessor_);
        std::swap(interpretation_, other.interpretation_);
    }

    /**
        A PersistentInterpretation<T> is valid if both accessor and
        interpretation are set.
    */
    operator bool() const
    {
        return accessor && interpretation;
    }

    ~PersistentInterpretation()
    {
        delete accessor;
    }
};


/**
    An Interpretation provides an interface to retrieve a value of the
    specified type T from a Token/WME.
    It serves some kind of type-erasure: Instead of checking if a given
    accessor is of some special type which provides a T (as in the old
    Accessor-System), you only need to check if the AccessorBase has an
    Interpretation<T> and use that. The Interpretation<T> is not independent
    of the specific accessor, though: It only stores a std::function that is
    bound to an implementation in a concrete accessor class.

    NOTE: The valuesEqual method assumes that the 'other' interpretation is
    of the *same type*!
*/
template <class T>
class Interpretation : public InterpretationBase {
    std::function<void(WME::Ptr, T&)> extractor_;
    AccessorBase* parent_;

public:

    /**
        A new instance of Interpretation must reference its parent-Accessor in
        order to get access to the index for the token to operate on, and also
        get a function to use in order to extract the value of type T from a
        WME.
    */
    Interpretation(AccessorBase* p, std::function<void(WME::Ptr, T&)> extr)
        : parent_(p), extractor_(extr)
    {
    }

    Interpretation(const Interpretation&) = delete;
    Interpretation& operator = (const Interpretation&) = delete;

    /**
        Accessor may have multiple interpretations, and nodes are usually
        only interested in one of them, but throughout their whole lifetime.

        This method provides an easy way to get a persistent copy of an
        accessor and its selected interpretation. It calls clone() on the
        accessor, grabs its Interpretation<T> and puts both in a
        PersistentInterpretation<T> object, which is only movable and destroys
        the contained pointers at the end of its lifetime.
    */
    PersistentInterpretation<T> makePersistent() const
    {
        auto a = parent_->clone();
        return PersistentInterpretation<T>(a);
    }

    /**
        Assumes the given WME is of a very specific type known to the specific
        Accessor that this interpretation is part of, and extracts the
        referenced value as a T.
    */
    void getValue(WME::Ptr wme, T& value) const
    {
        extractor_(wme, value);
    }

    /**
        Assumes that the WME in the token at the index specified by the accessor
        that this interpretation is part of is of the form required by
            void getValue(WME::Ptr, T&) const;
        and uses it to extract the value.
        Throws
            std::out_of_range
        if the token has not enough elements to index it,
        and a
            std::invalid_argument
        if the accessor is supposed to be used on single WMEs only (not tokens).
    */
    void getValue(Token::Ptr token, T& value) const
    {
        int count = parent_->index();
        if (count < 0)
        {
            throw std::invalid_argument(
                    "Accessor constructed for WMEs only applied to a Token");
        }

        while (count > 0 && token)
        {
            token = token->parent;
            count--;
        }

        if (!token)
        {
            throw std::out_of_range(
                    "Accessor indexes entry " + std::to_string(parent_->index())
                    + " and the token is " + std::to_string(count+1)
                    + " entries short on that.");
        }

        this->getValue(token->wme, value);
    }


    /**
        A safer way to get a value, supposed to be used by e.g. join nodes.
        Uses
            void getValue(WME::Ptr, T&) const
        if the accessor this belongs to has index < 0 and thus is supposed to
        be used on WMEs only, and
            void getValue(Token::Ptr, T&) const
        else.
    */
    void getValue(Token::Ptr token, WME::Ptr wme, T& value) const
    {
        if (parent_->index() < 0)
            getValue(wme, value);
        else
            getValue(token, value);
    }


    virtual bool valuesEqual(
                    const InterpretationBase& other,
                    Token::Ptr token,
                    WME::Ptr wme) const override
    {
        // assume only those get compared who access the same type!
        assert(typeid(this) == typeid(&other));

        auto o = static_cast<const Interpretation*>(&other);

        T otherVal, thisVal;
        o->getValue(token, wme, otherVal);
        this->getValue(token, wme, thisVal);

        return otherVal == thisVal;
    }
};




/// forward declaration of the templated Accessor class.
template <class T, class I = void, class... Is> class Accessor;

// recursion anchor - no more interpretations to add -> inherit AccessorBase.
template <class T>
class Accessor<T> : public AccessorBase {
};


/**
    This is a convenience class for the implementation of concrete accessors.
    Given a list of interpretations <I, Is...> that shall be possible to be
    retrieved from a WME type <T>, it adds pure virtual methods that need to be
    implemented in the concrete accessor class and are automatically registered
    with an Interpretation<I> in the AccessorBase.

    So, if you e.g. want to implement an accessor that accesses a single
    property in MyCoolWME and allows us to get it as a string or an int, you
    would define a class:

        class MyCoolWMEAccessor : public Accessor<MyCoolWME, string, int>

    and implement the pure virtual methods:

        void getValue(MyCoolWME*, string&) const;
        void getValue(MyCoolWME*, int&) const;
*/
template <class T, class I, class... Is>
class Accessor : public Accessor<T, Is...> {

    void getValueInternal(WME::Ptr wme, I& value) const
    {
        auto specificWME = static_cast<T*>(wme);
        getValue(specificWME, value);
    }

public:
    Accessor()
    {
        this->template interpretations_.push_back(
            {
            typeid(I),
            new Interpretation<I>(
                    std::bind(&Accessor::getValueInternal, this,
                              std::placeholders::_1,
                              std::placeholders::_2))
            });
    }

    // force user to implement how to get data of type I from a WME of type T
    virtual void getValue(std::shared_ptr<T>, I&) const = 0;
};



}


#endif /* end of include guard: RETE_VALUEACCESSOR_HPP_ */
