#include "../include/InferredEvidence.hpp"


namespace rete {

InferredEvidence::InferredEvidence(const Token::Ptr& token, const Production::Ptr& production)
    : token_(token), production_(production)
{
}

bool InferredEvidence::operator==(const Evidence& other) const
{
    auto o = dynamic_cast<const InferredEvidence*>(&other);
    if (!o) return false;
    // we get away with a ptr-check here, because these are exactly the pointer as propagated through the rete network
    return token_ == o->token_ && production_ == o->production_;
}

bool InferredEvidence::operator<(const Evidence& other) const
{
    auto o = dynamic_cast<const InferredEvidence*>(&other);
    if (!o)
    {
        // not same type, compare by pointer for some ordering..
        return this < &other;
    }
    else
    {
        // same type, compare by token and production
        if (production_ < o->production_) return true;
        if (production_ == o->production_)
        {
            // just pointer comparison!
            return token_ < o->token_;
        }
        return false;
    }
}

std::string InferredEvidence::toString() const
{
    return "Inferred by " + production_->getName() + " from " + token_->toString();
}

Token::Ptr InferredEvidence::token() const
{
    return token_;
}

} /* rete */
