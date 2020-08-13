#include "TokenGroupAccessor.hpp"

namespace rete {

TokenGroupAccessor* TokenGroupAccessor::clone() const
{
    auto ptr = new TokenGroupAccessor();
    ptr->index_ = index_;
    ptr->child_.reset(child_->clone());

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


}
