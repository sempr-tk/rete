#include "../include/AssertedEvidence.hpp"


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

std::string AssertedEvidence::toString() const
{
    return "Asserted(" + source_ + ")";
}

} /* rete */
