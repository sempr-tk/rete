#include "../include/BetaMemory.hpp"

namespace rete {

void BetaMemory::leftActivate(Token::Ptr t, WME::Ptr wme)
{
    Token::Ptr tNew(new Token());
    tNew->parent = t;
    tNew->wme = wme;

    tokens_.push_back(tNew);

    for (auto child : children_)
    {
        child->leftActivate(tNew);
    }
}

void BetaMemory::addChild(BetaNode::Ptr node)
{
    children_.push_back(node);
}

} /* rete */
