#include "AssertedEvidence.hpp"


namespace rete {

AssertedEvidence::AssertedEvidence(const std::string& source)
    : Evidence(AssertedEvidence::TypeId), source_(source)
{
}

bool AssertedEvidence::operator==(const Evidence& other) const
{
    if (this->type() != other.type()) return false;
    auto o = static_cast<const AssertedEvidence*>(&other);
    return o->source_ == source_;
}

bool AssertedEvidence::operator<(const Evidence& other) const
{
    if (this->type() != other.type()) return this->type() < other.type();
    // types are equal, static_cast okay!
    auto o = static_cast<const AssertedEvidence*>(&other);

    return this->source_ < o->source_;
}

std::string AssertedEvidence::toString() const
{
    return "Asserted(" + source_ + ")";
}

} /* rete */
