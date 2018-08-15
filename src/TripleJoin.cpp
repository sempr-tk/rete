#include "../include/TripleJoin.hpp"

namespace rete {

TripleJoin::TripleJoin(size_t tokenIndex, Triple::Field left, Triple::Field right)
    : tokenIndex_(tokenIndex), leftField_(left), rightField_(right)
{
}

bool TripleJoin::isValidCombination(Token::Ptr token, WME::Ptr rightWME)
{
    // travel along the token to the correct position
    size_t cnt = tokenIndex_;
    Token::Ptr refToken = token;
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
    return leftTriple->getField(leftField_) == rightTriple->getField(rightField_);
}

std::string TripleJoin::getDOTAttr() const
{
    std::string left = std::to_string(tokenIndex_) + "." + Triple::fieldName(leftField_);
    std::string right = Triple::fieldName(rightField_);
    return "[label=\"TripleJoin\\n" + left + " == " + right + "\"]";
}

} /* rete */
