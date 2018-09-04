#include "../include/JoinUnconditional.hpp"


namespace rete {

bool JoinUnconditional::isValidCombination(Token::Ptr, WME::Ptr)
{
    return true;
}

std::string JoinUnconditional::getDOTAttr() const
{
    return "[label=Unconditional]";
}

bool JoinUnconditional::operator== (const BetaNode& o) const
{
    auto other = dynamic_cast<const JoinUnconditional*>(&o);
    return other;
}

} /* rete */
