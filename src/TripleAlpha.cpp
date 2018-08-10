#include "../include/TripleAlpha.hpp"

namespace rete {

TripleAlpha::TripleAlpha(Triple::Field f, const std::string& v)
    : field_(f), value_(v)
{
}


void TripleAlpha::activate(WME::Ptr wme)
{
    if (Triple::Ptr triple = std::dynamic_pointer_cast<Triple>(wme))
    {
        if (triple->getField(field_) == value_) propagate(wme);
    }
}


bool TripleAlpha::operator == (const AlphaNode& other) const
{
    if (const TripleAlpha* o = dynamic_cast<const TripleAlpha*>(&other))
    {
        // equivalent if check on same field for same value
        return (o->field_ == field_) && (o->value_ == value_);
    }
    // not even a TripleAlpha node.
    return false;
}


} /* rete */
