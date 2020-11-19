#include "TokenGroupAccessor.hpp"

namespace rete {

TokenGroupAccessor::TokenGroupAccessor(std::unique_ptr<AccessorBase>&& child)
    : child_(std::move(child))
{
}

TokenGroupAccessor* TokenGroupAccessor::clone() const
{
    std::unique_ptr<AccessorBase> c(child_->clone());
    auto ptr = new TokenGroupAccessor(std::move(c));
    ptr->index_ = index_;

    return ptr;
}

bool TokenGroupAccessor::equals(const AccessorBase& other) const
{
    auto o = dynamic_cast<const TokenGroupAccessor*>(&other);
    return o != nullptr;
}

std::string TokenGroupAccessor::toString() const
{
    return "TokenGroup(" + std::to_string(index_) + ")";
}

void TokenGroupAccessor::getValue(TokenGroup::Ptr wme, TokenGroup::Ptr& value) const
{
    // looks stupid, but ... yeah. Identity is easy.
    // maybe this should be a default-implementation in Accessor<WMEType, ...>?
    value = wme;
}

const AccessorBase* TokenGroupAccessor::childAccessor() const
{
    return child_.get();
}



TokenGroupAccessorForwarder::TokenGroupAccessorForwarder(
        std::unique_ptr<AccessorBase>&& acc)
    : wrappedAccessor_(std::move(acc))
{
    // provide the exact same interpretations as the wrapped accessor
    this->interpretations_ = wrappedAccessor_->interpretations_;
    wrappedAccessor_->interpretations_.clear(); // prevent double delete

    // Make the interpretations use the index of this for the call to
    // getValue(Token::Ptr, T&) so that first the TokenGroup is selected.
    // In the subsesquent call to getValue(WME::Ptr, T&), first the
    // wmeModifier is applied which takes an entry of the TokenGroup and
    // uses the wrapped accessors index to select a WME that it can handle.
    for (auto ti : interpretations_)
    {
        ti.second->parent_ = this;

        // if the wmeModifier is already set we may be stacking
        // TokenGroupAccessorForwarders! Multiple usage of GROUP BY in a
        // single rule!
        // To support this, we need to first extract ourselves, and then
        // apply the modifier for what we extracted.
        if (ti.second->wmeModifier_)
        {
            auto inner = ti.second->wmeModifier_;
            ti.second->wmeModifier_ =
                [inner, this](WME::Ptr wme) -> WME::Ptr
                {
                    auto tmp = this->getWMEFromTokenGroup(wme);
                    return inner(tmp);
                };
        }
        else
        {
            // make the wrapped accessor work on the tokengroups entries
            ti.second->wmeModifier_ =
                std::bind(&TokenGroupAccessorForwarder::getWMEFromTokenGroup,
                      this, std::placeholders::_1);
        }
    }
}

WME::Ptr TokenGroupAccessorForwarder::getWMEFromTokenGroup(WME::Ptr wme) const
{
    auto tg = std::static_pointer_cast<TokenGroup>(wme);
    if (tg->token_.empty())
        throw std::exception(); // there must be no empty groups

    auto token = *tg->token_.begin();
    int count = wrappedAccessor_->index();

    while (count > 0 && token)
    {
        token = token->parent;
        count--;
    }

    if (!token)
    {
        throw std::out_of_range(
            "Accessor indexes entry " + std::to_string(wrappedAccessor_->index())
            + " and the token is " + std::to_string(count+1)
            + " entries short on that.");
    }
    else
    {
        return token->wme;
    }
}


bool TokenGroupAccessorForwarder::equals(const AccessorBase& other) const
{
    auto o = dynamic_cast<const TokenGroupAccessorForwarder*>(&other);
    if (o)
        return *wrappedAccessor_ == *o->wrappedAccessor_;
    else
        return false;
}


TokenGroupAccessorForwarder* TokenGroupAccessorForwarder::clone() const
{
    std::unique_ptr<AccessorBase> wrapped(wrappedAccessor_->clone());
    auto p = new TokenGroupAccessorForwarder(std::move(wrapped));
    p->index_ = this->index_;
    return p;
}

std::string TokenGroupAccessorForwarder::toString() const
{
    return "{forwarded: " + wrappedAccessor_->toString() + "}";
}



Interpretation<TokenGroup::Ptr>::Interpretation(
        AccessorBase* p,
        std::function<void(WME::Ptr, TokenGroup::Ptr&)> extr)
    :
        InterpretationImpl<TokenGroup::Ptr>(p, extr)
{
}


}
