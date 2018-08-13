#include "../include/BetaMemory.hpp"
#include "../include/BetaNode.hpp"

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

void BetaMemory::getChildren(std::vector<BetaNode::Ptr>& children)
{
    children.reserve(children_.size());
    for (auto c : children_) children.push_back(c);
}

size_t BetaMemory::size() const
{
    return tokens_.size();
}

BetaMemory::Iterator BetaMemory::begin()
{
    return tokens_.begin();
}

BetaMemory::Iterator BetaMemory::end()
{
    return tokens_.end();
}

std::string BetaMemory::getDOTAttr() const
{
    std::string record = "";
    for (auto t : tokens_)
    {
        record += "|" + t->toString();
    }
    return "[shape=record, label=\"{BetaMemory" + record + "}\"]";
}

} /* rete */
