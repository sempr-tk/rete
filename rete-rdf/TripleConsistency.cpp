#include "TripleConsistency.hpp"

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
    return "[label=\"TripleCheck\\n(" + Triple::fieldName(field1_) + " == " + Triple::fieldName(field2_) + ")\"]";
}

} /* rete */
