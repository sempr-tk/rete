#ifndef RETE_TOKENGROUPACCESSOR_HPP_
#define RETE_TOKENGROUPACCESSOR_HPP_

#include "TokenGroup.hpp"
#include "Accessors.hpp"

namespace rete {


/**
    The TokenGroupAccessor does not return any other value than the TokenGroup
    itself. Its main characteristic is the fact that is holds another accessor,
    the original one which can be applied to the tokens in the TokenGroup.
*/
class TokenGroupAccessor : public Accessor<TokenGroup, TokenGroup::Ptr> {
    std::unique_ptr<AccessorBase> child_;

    bool equals(const AccessorBase& other) const override;
public:
    TokenGroupAccessor(std::unique_ptr<AccessorBase>&& child);

    TokenGroupAccessor* clone() const override;
    std::string toString() const override;
    void getValue(TokenGroup::Ptr, TokenGroup::Ptr& value) const override;
    const AccessorBase* childAccessor() const;
};



/**
    The TokenGroupAccessorForwarder is an Accessor that tricks a bit in order
    to expose exactly the interface of a wrapped accessor, but instead of
    applying it directly on a given token it first extracts a TokenGroup from
    the token and then applies the wrapped accessor to the first entry in the
    TokenGroup.

    It makes the encapsulation of matches in a TokenGroup transparent, which
    is needed to access those variables on which groups are created.

    The actual implementation is a bit tricky. The AccessorBase provides
    template methods to get interpretations etc, which cannot be overridden.
    Hence we need to modify the internal representation manually, such that
    the templates in AccessorBase yield correct results. This means providing
    the same (identical!) interpretations, but modified in a way that they
    do this extra step.

*/
class TokenGroupAccessorForwarder : public AccessorBase {
    std::unique_ptr<AccessorBase> wrappedAccessor_;

    /**
        Assumes WME to be a TokenGroup, takes the first entry from the group
        (a token), returns the WME located at the wrapped accessors index.
    */
    WME::Ptr getWMEFromTokenGroup(WME::Ptr) const;

    bool equals(const AccessorBase& other) const override;
public:
    using Ptr = std::shared_ptr<TokenGroupAccessorForwarder>;

    TokenGroupAccessorForwarder(std::unique_ptr<AccessorBase>&&);
    TokenGroupAccessorForwarder* clone() const override;
    std::string toString() const override;

};



/**
    Specialization of Interpretation for TokenGroups. These grant access to
    the child-interpretation, i.e. the accessor and its interpretations
    inside a token of the TokenGroup.
*/
template <>
class Interpretation<TokenGroup::Ptr> : public InterpretationImpl<TokenGroup::Ptr> {
public:
    Interpretation(AccessorBase* p,
                   std::function<void(WME::Ptr, TokenGroup::Ptr&)> extr);


    const AccessorBase* childAccessor() const
    {
        return static_cast<TokenGroupAccessor*>(this->parent_)->childAccessor();
    }

    template <class I>
    const Interpretation<I>* getChildInterpretation() const
    {
        auto acc = static_cast<TokenGroupAccessor*>(this->parent_)->childAccessor();
        return acc->getInterpretation<I>();
    }
};



}

#endif /* include guard: RETE_TOKENGROUPACCESSOR_HPP_ */
