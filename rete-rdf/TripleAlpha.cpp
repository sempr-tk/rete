#include "TripleAlpha.hpp"
#include "../rete-core/Util.hpp"

namespace rete {

TripleAlpha::TripleAlpha(Triple::Field f, const std::string& v)
    : field_(f), value_(v)
{
}


void TripleAlpha::activate(WME::Ptr wme, PropagationFlag flag)
{
    if (flag == PropagationFlag::RETRACT)
    {
        // shortcut: without any (expensive) check, just propagate!
        propagate(wme, PropagationFlag::RETRACT);
    }
    else if (flag == PropagationFlag::ASSERT)
    {
        // static cast: assume there has been a TripleTypeAlpha-node that
        // already checked the type!
        auto triple = std::static_pointer_cast<Triple>(wme);

        if (triple->getField(field_) == value_) propagate(wme, flag);
    }
    else if (flag == PropagationFlag::UPDATE)
    {
        // static cast: assume there has been a TripleTypeAlpha-node that
        // already checked the type!
        auto triple = std::static_pointer_cast<Triple>(wme);

        if (triple->getField(field_) == value_) propagate(wme, PropagationFlag::UPDATE);
        else propagate(wme, PropagationFlag::RETRACT);
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

std::string TripleAlpha::getDOTAttr() const
{
    std::string field = Triple::fieldName(field_);
    return "[label=\"TripleCheck\\n (" + util::dotEscape(field) + " == " +
            util::dotEscape(value_) + ")\"]";
}



} /* rete */
