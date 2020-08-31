#include "TripleAccessor.hpp"
#include <sstream>

rete::TripleAccessor::TripleAccessor(rete::Triple::Field field)
    : field_(field)
{
}

void rete::TripleAccessor::getValue(rete::Triple::Ptr wme, std::string& value) const
{
    auto triple = std::static_pointer_cast<rete::Triple>(wme);
    value = triple->getField(field_);
}


bool rete::TripleAccessor::equals(const rete::AccessorBase& other) const
{
    auto o = dynamic_cast<const rete::TripleAccessor*>(&other);
    if (!o) return false;;
    if (this->field_ == o->field_) return true;
    return false;
}

void rete::TripleAccessor::getValue(rete::Triple::Ptr wme, float& value) const
{
    std::string str;
    getValue(wme, str);

    std::istringstream(str) >> value;
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
    auto ptr = new rete::TripleAccessor(this->field_);
    ptr->index() = index_;
    return ptr;
}
