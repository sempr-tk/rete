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
    return token_ == o->token_ && production_ == o->production_;
}

std::string InferredEvidence::toString() const
{
    return "Inferred by " + production_->getName() + " from " + token_->toString();
}

} /* rete */
