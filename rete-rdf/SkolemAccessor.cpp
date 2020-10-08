#include "SkolemAccessor.hpp"

namespace rete {

void SkolemAccessor::getValue(Skolem::Ptr wme, TriplePart& value) const
{
    getValue(wme, value.value);
}

void SkolemAccessor::getValue(Skolem::Ptr wme, std::string& value) const
{
    value = wme->identifier;
}

bool SkolemAccessor::equals(const AccessorBase& other) const
{
    auto o = dynamic_cast<const SkolemAccessor*>(&other);
    if (!o) return false;
    else    return true;
}

std::string SkolemAccessor::toString() const
{
    return "Skolem" +
            (index_ < 0 ?
                std::string("") :
                "[" + std::to_string(index_) + "]");
}


SkolemAccessor* SkolemAccessor::clone() const
{
    auto ptr = new SkolemAccessor();
    ptr->index() = index_;
    return ptr;
}

}
