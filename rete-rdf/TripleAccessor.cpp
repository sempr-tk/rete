#include "TripleAccessor.hpp"
#include <sstream>
#include <iomanip>

rete::TripleAccessor::TripleAccessor(rete::Triple::Field field)
    : field_(field)
{
}

void rete::TripleAccessor::getValue(rete::Triple::Ptr wme, std::string& value) const
{
    std::string raw = wme->getField(field_);
    if (raw.size() >= 2 &&
        *raw.begin() == '\"' &&
        *raw.rbegin() == '\"')
    {
        // quoted -> remove quotes to get the string
        std::stringstream ss(raw);
        ss >> std::quoted(value);
    }
    else if (raw.size() >= 2 &&
            *raw.begin() == '<' &&
            *raw.rbegin() == '>')
    {
        // a resource -> remove the brackets.
        value = raw.substr(1, raw.size()-2);
    }
    else
    {
        // ... it wasn't really something well formed that should be interpreted
        // as a string, right? No quotes, no <...>, just.. something. Maybe
        // a plain number? Whatever. Just output the raw thing.
        value = raw;
    }
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

void rete::TripleAccessor::getValue(rete::Triple::Ptr wme, TriplePart& value) const
{
    value.value = wme->getField(field_);
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
