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
#include <stdexcept>
#include <cxxabi.h>

#include "WME.hpp"
#include "Token.hpp"
#include "Util.hpp"

namespace rete {

// forward declaration of Interpretation<T>
template <class T> class InterpretationImpl;
template <class T> class Interpretation;

// ... and of PersistentInterpretation<T>
template <class T> struct PersistentInterpretation;
class AccessorBase;


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
protected:
    AccessorBase* parent_;
    /// Introduced to make changes to the given WME before extracting a value
    /// from it. Only set by the TokenGroupAccessorForwarder.
    std::function<WME::Ptr(WME::Ptr)> wmeModifier_;
    friend class TokenGroupAccessorForwarder;
public:
    InterpretationBase(AccessorBase* ab)
        : parent_(ab)
    {
    }

    virtual ~InterpretationBase() = default;

    /**
        Casts other to the same Interpretation<T> as this (statically!) and
        checks if the values returned by this and other applied to the given
        token/wme are equal.
    */
    virtual bool valuesEqual(const InterpretationBase& other,
                             Token::Ptr token,
                             WME::Ptr wme) const = 0;

    /**
        Checks if the values accessed by this in two tokens are equal.
        (Note: Implemented specifically for GroupBy nodes)
    */
    virtual bool valuesEqual(Token::Ptr t1, Token::Ptr t2) const = 0;

    /**
        For debugging purposes: Returns a string describing the type of
        interpretation done, the T in Interpretation<T>.
    */
    virtual std::string internalTypeName() const = 0;

    /**
        Checks if this interpretation is for one of the given types.
    */
    template <class... Ts> bool isOneOf() const
    {
        return rete::util::IsOneOf<Interpretation<Ts>...>()(this);
    }

};


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
    friend class TokenGroupAccessorForwarder;
public:
    typedef std::pair<std::type_index, InterpretationBase*> TypeInterpretationPair;
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
        Returns the Interpretation<X> where X is the first intepretation
        available that is from the given set of types {T, Ts...}.

        If a node can e.g. support to work with int, float and string, it can
        get the one interpretation that is preferred by the accessor by calling
            getPreferredInterpretation<int, float, string>()
        So that if the original value is e.g. an int we will use that
        interpretation.

        Since this decision happens at runtime there is no way to make this
        typesafe. You'll get a const InterpretationBase* as a result (or a
        nullptr if no match is found), and have to check for the actual type
        dynamically.
    */
    template <class... Ts>
    const InterpretationBase* getPreferredInterpretation() const
    {
        for (auto interpretation : interpretations_)
        {
            if (interpretation.second->isOneOf<Ts...>())
            {
                return interpretation.second;
            }
        }
        return nullptr;
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

protected:
    int index_;
    /// A vector containing all possible interpretations of the accessed value
    std::vector<TypeInterpretationPair> interpretations_;
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
        : accessor_(nullptr), interpretation_(nullptr)
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
class InterpretationImpl : public InterpretationBase {
protected:
    std::function<void(WME::Ptr, T&)> extractor_;

public:

    /**
        A new instance of Interpretation must reference its parent-Accessor in
        order to get access to the index for the token to operate on, and also
        get a function to use in order to extract the value of type T from a
        WME.
    */
    InterpretationImpl(AccessorBase* p, std::function<void(WME::Ptr, T&)> extr)
        : InterpretationBase(p), extractor_(extr)
    {
    }

    InterpretationImpl(const InterpretationImpl&) = delete;
    InterpretationImpl& operator = (const InterpretationImpl&) = delete;

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
        if (this->wmeModifier_)
            wme = this->wmeModifier_(wme);
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


    bool valuesEqual(
                    const InterpretationBase& other,
                    Token::Ptr token,
                    WME::Ptr wme) const override
    {
        // assume only those get compared who access the same type!
        assert(typeid(*this) == typeid(other));

        auto o = static_cast<const InterpretationImpl*>(&other);

        T otherVal, thisVal;
        o->getValue(token, wme, otherVal);
        this->getValue(token, wme, thisVal);

        return otherVal == thisVal;
    }

    bool valuesEqual(Token::Ptr t1, Token::Ptr t2) const override
    {
        T val1, val2;
        getValue(t1, val1);
        getValue(t2, val2);

        return val1 == val2;
    }

    std::string internalTypeName() const override
    {
        return util::beautified_typename<T>().value;
    }
};


/// the default Integration<T> is exactly IntegrationImpl<T>.
template <class T>
class Interpretation : public InterpretationImpl<T> {
public:
    Interpretation(AccessorBase* p, std::function<void(WME::Ptr, T&)> extr)
        : InterpretationImpl<T>(p, extr)
    {
    }
};




/// forward declaration of the templated Accessor class.
template <class T, class I = void, class... Is> class Accessor;

/*
// recursion anchor - no more interpretations to add -> inherit AccessorBase.
template <class T>
class Accessor<T> : public AccessorBase {
};
*/

/**
    Recursion anchor - only one interpretation is left to add.

    This adds some redundant code, and I'd prefer to have the recursion achor
    at 0 interpretations left to add. But we need the using statement to import
    the methods defined in the base classes, and in that case the Accessor<T>
    has no such method, resulting in a compiler error.
    So, anchor at 1, and only leave out the using directive.
*/
template <class T, class I>
class Accessor<T, I> : public AccessorBase {

    void getValueInternal(WME::Ptr wme, I& value) const
    {
        auto specificWME = std::static_pointer_cast<T>(wme);
        getValue(specificWME, value);
    }

public:
    Accessor()
    {
        this->interpretations_.insert(
                this->interpretations_.begin(),
                {
                typeid(I),
                new Interpretation<I>(this,
                        std::bind(&Accessor::getValueInternal, this,
                            std::placeholders::_1,
                            std::placeholders::_2))
                });
    }

    virtual void getValue(std::shared_ptr<T>, I&) const = 0;

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
        auto specificWME = std::static_pointer_cast<T>(wme);
        getValue(specificWME, value);
    }

public:
    Accessor()
    {
        this->template interpretations_.insert(
            this->template interpretations_.begin(),
            {
            typeid(I),
            new Interpretation<I>(this,
                    std::bind(&Accessor::getValueInternal, this,
                              std::placeholders::_1,
                              std::placeholders::_2))
            });
    }

    // pull getValue with different signatures from base classes to avoid
    // "hidden overloaded virtual"
    using Accessor<T, Is...>::getValue;

    // force user to implement how to get data of type I from a WME of type T
    virtual void getValue(std::shared_ptr<T>, I&) const = 0;
};


/**
    A utility to create accessors to constant values independent of actual WMEs.
*/
template <class I>
class ConstantAccessor : public Accessor<WME, I> {
    I value_;
public:
    ConstantAccessor(I v)
        : value_(v)
    {
    }

    void getValue(WME::Ptr, I& v) const override
    {
        v = value_;
    }

    ConstantAccessor* clone() const override
    {
        auto ptr = new ConstantAccessor(value_);
        ptr->index_ = this->index_;
        return ptr;
    }

    bool equals(const AccessorBase& other) const override
    {
        auto o = dynamic_cast<const ConstantAccessor*>(&other);
        if (!o)
            return false;
        else
            return o->value_ == this->value_;
    }

    std::string toString() const override
    {
        return util::to_string(value_);
    }
};



/**
    A utility to create conversion objects that help supporting multiple
    interpretations in a Node/NodeBuilder that all need to be converted to the
    same type. E.g., the TripleEffect-Node stores everything as string, but
    needs to support TriplePart, string, float, int, ... and convert them to
    strings, sometimes quoting them (for string literals), or putting them in
    brackets (for resources), ...
*/
template <class Destination, class S = void, class... Sources> class AccessorConversion;


template <class D, class S, class... Ss>
class AccessorConversion : public AccessorConversion<D, Ss...> {
    /**
        A getter to retrieve S from a wme --
        nullptr if S is not the preferred interpretation of the accessor in the
        set {S, Ss...}.

        Explanation:
        Every subclass of AccessorConversion gets the preferred interpretation
        of the accessor under the assumption that only the type S (implemented
        by this class) and the types Ss... (implemented by the inherited
        AccessorConversion<D, Ss...>) are supported.
        If from this set of types S is actually the preferred we mark this by
        selecting the get-function of this class as the one to use.
        If not, leave everything as it is -- either none of the types so far
        is supported by the accessor, or a previous type from Ss... is preferred
        over S.
        Since the ctor of the base class is evaluated first we start with the
        minimal set of one type to check, and with every layer of this template
        extend it by a type and only change the selected getter if the added
        type if preferred over all previous types.
    */
    const Interpretation<S>* getter_;

    void get(WME::Ptr wme, D& destination) const
    {
        S source;
        getter_->getValue(wme, source);
        convert(source, destination);
    }

    std::string toStringImpl() const
    {
        return this->parent_->toString() +
            " [" + util::beautified_typename<S>().value + " -> " +
                   util::beautified_typename<D>().value + "]";
    }


    /**
        Checks the accessor for its preferred interpretation and sets the
        function pointers in the base class of this to special methods of this
        class if it matches the interpretation supported by it.
    */
    void init()
    {
        if (!this->destDirectlyAvailable_)
        {
            const InterpretationBase* i =
                this->accessorKeepAlive_->template getPreferredInterpretation<S, Ss...>();

            getter_ = dynamic_cast<const Interpretation<S>*>(i);
            if (getter_)
            {
                this->extractor_ = std::bind(&AccessorConversion::get,
                        this,
                        std::placeholders::_1,
                        std::placeholders::_2);
                this->toString_ = std::bind(&AccessorConversion::toStringImpl, this);
            }
        }
    }

protected:
    // create an invalid/empty conversion
    AccessorConversion()
        : AccessorConversion<D, Ss...>()
    {
    }

    AccessorConversion(std::unique_ptr<AccessorBase>&& base)
        : AccessorConversion<D, Ss...>(std::move(base))
    {
        init();
    }

    AccessorConversion(const AccessorConversion&) = delete;
    AccessorConversion& operator = (const AccessorConversion&) = delete;

    AccessorConversion(AccessorConversion&& other)
        : AccessorConversion<D, Ss...>(std::move(other))
    {
        init();
    }

    /**
        The actual conversion method to be implemented by the user.
    */
    virtual void convert(const S& source, D& destination) const = 0;

    // pull the other conversion methods, avoid hiding overloaded virtual
    using AccessorConversion<D, Ss...>::convert;
};

/**
    Recursion anchor. Reuses Interpretation<D> for the token-iterating stuff,
    but should not be seen as one. The AccessorConversion actually owns the
    accessor it is based on, which is not the case for interpretations.

    Also, adds support for D->D conversion and a reduced toString method.
*/
template <class D>
class AccessorConversion<D> : protected Interpretation<D> {
protected:
    std::unique_ptr<AccessorBase> accessorKeepAlive_;
    std::function<std::string(void)> toString_;

    /**
        If the destination type D is already supported by the accessor it is
        to be used! Even if it is not the preferred datatype of the accessor.
        Using a not-preferred type here means just letting the source decide
        how to convert to the destination type, instead of getting a more
        "preferred" type from the accessor (potentially already converting) and
        then providing our own conversion to the destination type.

        This flag is set if the destination type D is supported by the accessor
        and prevents the function pointers from being overwritten by subclasses.
    */
    bool destDirectlyAvailable_;

    // Just to make the compiler shut up.
    // In derived classes there is a "using ___::convert" statement to silence
    // the "hiding overloaded virtual" warning, but this base class does not
    // have a meaningful convert function. And without one, we get a compiler
    // error. So here it is, basic, small, and meaningless.
    void convert() const {}

private:
    const Interpretation<D>* getter_;

    void get(WME::Ptr wme, D& destination) const
    {
        getter_->getValue(wme, destination);
    }

    std::string toStringImpl() const
    {
        return this->parent_->toString() +
               " [" + util::beautified_typename<D>().value + "]";
    }

    /**
        Checks the accessor for its preferred interpretation and sets the
        function pointers in the base class of this to special methods of this
        class if it matches the interpretation supported by it.
    */
    void init()
    {
        getter_ = this->accessorKeepAlive_->template getInterpretation<D>();
        if (getter_)
        {
            this->extractor_ = std::bind(&AccessorConversion::get,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2);
            this->toString_ = std::bind(&AccessorConversion::toStringImpl, this);
            destDirectlyAvailable_ = true;
        }
        else
        {
            destDirectlyAvailable_ = false;
        }
    }

public:
    // creates an invalid conversion
    AccessorConversion()
        : Interpretation<D>(nullptr, nullptr),
          accessorKeepAlive_(nullptr)
    {
    }

    AccessorConversion(std::unique_ptr<AccessorBase>&& accessor)
        : Interpretation<D>(accessor.get(), nullptr),
          accessorKeepAlive_(std::move(accessor))
    {
        init();
    }

    // not copyable. has a unique_ptr, so these should be implicitely
    // deleted either way.
    AccessorConversion(const AccessorConversion&) = delete;
    AccessorConversion& operator = (const AccessorConversion&) = delete;

    bool hasEqualAccessor(const AccessorConversion& other) const
    {
        return (*this->accessorKeepAlive_ == *other.accessorKeepAlive_);
    }

    /**
        Move-ctor of AccessorConversion. Moves the keep-alive-ptr to the
        accessor, but uses the usual (not move!)-ctor of Interpretation<D>
        to set std::functions to nullptr. Those must neither be moved nor
        copied, as they are bound to the "other" object which is probably
        deleted! Instead, the ctor-chain needs to do the same checks as in
        the normal ctor again to set these functions.
    */
    AccessorConversion(AccessorConversion&& other)
        : Interpretation<D>(other.parent_, nullptr),
          toString_(nullptr)
    {
        accessorKeepAlive_ = std::move(other.accessorKeepAlive_);
        init();
    }

    // ... don't really need the move assignment.
    // Just adds more ways to create errors. A lot of care needs to be taken to
    // not let the std::functions point to deleted objects, which I only want
    // to do once.
    AccessorConversion& operator = (AccessorConversion&&) = delete;

    /**
        Users of AccessorConversion basically make use of the
        Interpretation<D>::getValue, just that the extractor-function in the
        Interpretation has been set to a getter/conversion function of the
        AccessorConversion
    */
    using Interpretation<D>::getValue;

    std::string toString() const
    {
        if (toString_)
            return toString_();
        else
            return "";
    }

    /**
        Quick check if the conversion object is valid: If the extractor
        variable wasn't set, the given accessor does not support any of the
        source datatypes.
    */
    operator bool () const
    {
        return this->extractor_ && this->toString_;
    }
};


}


#endif /* end of include guard: RETE_VALUEACCESSOR_HPP_ */
