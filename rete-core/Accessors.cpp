#include "Accessors.hpp"

namespace rete {

Accessor::Accessor(int index)
    : index_(index)
{
}

Accessor::~Accessor()
{
}

// --------------------------------------
//  StringAccessor
// --------------------------------------
StringAccessor::StringAccessor()
{
    registerType<StringAccessor>();
}

bool StringAccessor::canCompareValues(const Accessor& other) const
{
    return other.canAs<StringAccessor>();
}

bool StringAccessor::valuesEqual(Accessor& other, Token::Ptr token, WME::Ptr wme)
{
    auto optr = other.as<StringAccessor>();
    std::string myValue, otherValue;
    if (index() == -1) this->getValue(wme, myValue);
    else               this->getValue(token, myValue);

    if (optr->index() == -1) optr->getValue(wme, otherValue);
    else                     optr->getValue(token, otherValue);

    return myValue == otherValue;
}

std::string StringAccessor::getString(WME::Ptr wme)
{
    std::string tmp;
    getValue(wme, tmp);
    return tmp;
}

std::string StringAccessor::getString(Token::Ptr token)
{
    std::string tmp;
    getValue(token, tmp);
    return tmp;
}


// --------------------------------------
//  NumberAccessor
// --------------------------------------
NumberAccessor::NumberAccessor()
{
    registerType<NumberAccessor>();
}

bool NumberAccessor::canCompareValues(const Accessor& other) const
{
    return other.canAs<NumberAccessor>();
}

bool NumberAccessor::valuesEqual(Accessor& other, Token::Ptr token, WME::Ptr wme)
{
    auto optr = other.as<NumberAccessor>();

    // TODO: equality check on doubles isn't cool. expand these checks to take datatype into account!
    double myVal, otherVal;


    if (index() == -1) this->getValue(wme, myVal);
    else               this->getValue(token, myVal);

    if (optr->index() == -1) optr->getValue(wme, otherVal);
    else                     optr->getValue(token, otherVal);

    return myVal == otherVal;
}

} /* rete */
