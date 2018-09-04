#include "../include/TripleJoin.hpp"

namespace rete {

TripleJoin::TripleJoin(size_t tokenIndex, Triple::Field left, Triple::Field right)
{
    checks_.push_back(check_t{tokenIndex, left, right});
}

TripleJoin::TripleJoin()
{
}

void TripleJoin::addCheck(check_t check)
{
    checks_.push_back(check);
}

bool TripleJoin::performCheck(check_t& check, Token::Ptr leftToken, WME::Ptr rightWME) const
{
    size_t cnt = std::get<0>(check);
    Triple::Field leftField = std::get<1>(check);
    Triple::Field rightField = std::get<2>(check);

    Token::Ptr refToken = leftToken;
    while (cnt > 0 && refToken)
    {
        refToken = refToken->parent;
    }

    // if the token index is wrong the network is invalid...
    if (!refToken) throw std::exception();

    auto leftWME = refToken->wme;

    // cast both to Triple. This could be a static cast, since the topology of the nework should
    // assert that only triples arrive here... but lets be sure to catch the errors:
    auto leftTriple = std::dynamic_pointer_cast<Triple>(leftWME);
    // again, if any of the two is not a triple, the whole network is ill-formed.
    if (!leftTriple) throw std::exception();

    auto rightTriple = std::dynamic_pointer_cast<Triple>(rightWME);
    if (!rightTriple) throw std::exception();

    // now, lets do the actual check:
    return leftTriple->getField(leftField) == rightTriple->getField(rightField);
}

bool TripleJoin::isValidCombination(Token::Ptr token, WME::Ptr rightWME)
{
    for (auto c : checks_)
    {
        if (!performCheck(c, token, rightWME)) return false;
    }
    return true;
}

std::string TripleJoin::getDOTAttr() const
{
    std::string label = "[label=\"TripleJoin";
    for (auto c : checks_)
    {
        label += "\\n";
        std::string left = std::to_string(std::get<0>(c)) + "." + Triple::fieldName(std::get<1>(c));
        std::string right = Triple::fieldName(std::get<2>(c));
        label += left + " == " + right;
    }
    label += "\"]";

    return label;
}

bool TripleJoin::operator == (const BetaNode& other) const
{
    auto otherJoin = dynamic_cast<const TripleJoin*>(&other);
    if (!otherJoin) return false;

    if (otherJoin->checks_.size() != this->checks_.size()) return false;
    for (size_t i = 0; i < checks_.size(); i++)
    {
        if (otherJoin->checks_[i] != this->checks_[i]) return false;
    }

    return true;
}

} /* rete */
