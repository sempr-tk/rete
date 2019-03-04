#include "TripleAccessor.hpp"
#include <sstream>

rete::TripleAccessor::TripleAccessor(rete::Triple::Field field)
    : field_(field)
{
    registerType<rete::TripleAccessor>();
}

void rete::TripleAccessor::getValue(rete::WME::Ptr wme, std::string& value) const
{
    auto triple = std::static_pointer_cast<rete::Triple>(wme);
    value = triple->getField(field_);
}


bool rete::TripleAccessor::equals(const rete::Accessor& other) const
{
    auto o = dynamic_cast<const rete::TripleAccessor*>(&other);
    if (!o) return false;;
    if (this->field_ == o->field_) return true;
    return false;
}

float rete::TripleAccessor::internalValue(WME::Ptr wme) const
{
    std::string str;
    getValue(wme, str);

    float value;
    std::istringstream(str) >> value;

    return value;
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


bool rete::TripleAccessor::canCompareValues(const Accessor& other) const
{
    // need at least a StringAccessor to compare with.
    // (so far, everything is at least a StringAccessor...)
    return other.canAs<StringAccessor>();
}

#include <iostream>
bool rete::TripleAccessor::valuesEqual(Accessor& other, Token::Ptr token, WME::Ptr wme)
{
    std::cout << "Values equal?" << std::endl;
    // if the other is a TripleAccessor, too, don't try to interpret values as numbers!
    // TODO: There should be something more sophisticated here?
    if (other.canAs<TripleAccessor>())
        return this->StringAccessor::valuesEqual(other, token, wme);

    // if the other one is a NumberAccessor, but not a TripleAccessor, try to compare numbers
    if (this->NumberAccessor::canCompareValues(other))
        return this->NumberAccessor::valuesEqual(other, token, wme);

    // else just use strings
    return this->StringAccessor::valuesEqual(other, token, wme);
}
