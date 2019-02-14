#include "TripleAccessor.hpp"

rete::TripleAccessor::TripleAccessor(rete::Triple::Field field)
    : field_(field)
{
}

std::string rete::TripleAccessor::value(rete::WME::Ptr wme) const
{
    auto triple = std::static_pointer_cast<rete::Triple>(wme);
    return triple->getField(field_);
}


bool rete::TripleAccessor::equals(const rete::Accessor& other) const
{
    auto o = dynamic_cast<const rete::TripleAccessor*>(&other);
    if (!o) return false;;
    if (this->field_ == o->field_) return true;
    return false;
}


std::string rete::TripleAccessor::toString() const
{
    return "Triple" +
                (index_ < 0 ? std::string("") :
                              "[" + std::to_string(index_) + "]") +
                "." + rete::Triple::fieldName(field_);
}

rete::TripleAccessor* rete::TripleAccessor::clone() const
{
    return new rete::TripleAccessor(*this);
}
