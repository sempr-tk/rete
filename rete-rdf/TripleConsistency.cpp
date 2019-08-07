#include "TripleConsistency.hpp"

namespace rete {

TripleConsistency::TripleConsistency(Triple::Field a, Triple::Field b)
    : field1_(a), field2_(b)
{
}


void TripleConsistency::activate(WME::Ptr wme, PropagationFlag flag)
{
    Triple::Ptr t = std::dynamic_pointer_cast<Triple>(wme);

    if (flag == rete::RETRACT)
    {
        if (t) propagate(wme, flag);
    }
    else if (flag == rete::ASSERT)
    {
        // the actual check.
        if (t && t->getField(field1_) == t->getField(field2_)) propagate(wme, flag);
    }
    else if (flag == rete::UPDATE)
    {
        if (t && t->getField(field1_) == t->getField(field2_)) propagate(wme, rete::UPDATE);
        else propagate(wme, rete::RETRACT);
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
