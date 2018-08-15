#include "../include/TripleConsistency.hpp"

namespace rete {

TripleConsistency::TripleConsistency(Triple::Field a, Triple::Field b)
    : field1_(a), field2_(b)
{
}


void TripleConsistency::activate(WME::Ptr wme, PropagationFlag flag)
{
    Triple::Ptr t = std::dynamic_pointer_cast<Triple>(wme);
    if (!t) return;

    if (flag == rete::RETRACT)
    {
        propagate(wme, flag);
        return;
    }
    else if (flag == rete::ASSERT)
    {
        // the actual check.
        if (t->getField(field1_) == t->getField(field2_)) propagate(wme, flag);
    }
}

bool TripleConsistency::operator == (const AlphaNode& other) const
{
    if (this == &other) return true;
    auto o = dynamic_cast<const TripleConsistency*>(&other);
    if (!o) return false;
    return  (o->field1_ == field1_ && o->field2_ == field2_) ||
            (o->field1_ == field2_ && o->field2_ == field1_);
}

std::string TripleConsistency::getDOTAttr() const
{
    auto conv = [](Triple::Field f) -> std::string
    {
        if (f == Triple::SUBJECT) return "?sub";
        if (f == Triple::PREDICATE) return "?pred";
        if (f == Triple::OBJECT) return "?obj";
        return "";
    };

    return "[label=\"TripleCheck\\n(" + conv(field1_) + " == " + conv(field2_) + ")\"]";
}

} /* rete */
