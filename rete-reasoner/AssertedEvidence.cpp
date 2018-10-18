#include "AssertedEvidence.hpp"


namespace rete {

AssertedEvidence::AssertedEvidence(const std::string& source)
    : source_(source)
{
}

bool AssertedEvidence::operator==(const Evidence& other) const
{
    auto o = dynamic_cast<const AssertedEvidence*>(&other);
    if (!o) return false;
    return o->source_ == source_;
}

bool AssertedEvidence::operator<(const Evidence& other) const
{
    auto o = dynamic_cast<const AssertedEvidence*>(&other);
    if (!o)
    {
        // not of same type: compare pointer for some ordering..
        return this < &other;
    }
    else
    {
        return this->source_ < o->source_;
    }
}

std::string AssertedEvidence::toString() const
{
    return "Asserted(" + source_ + ")";
}

} /* rete */
