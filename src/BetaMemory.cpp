#include "../include/BetaMemory.hpp"
#include "../include/BetaNode.hpp"

#include <algorithm>

namespace rete {

void BetaMemory::leftActivate(Token::Ptr t, WME::Ptr wme, PropagationFlag flag)
{
    if (flag == PropagationFlag::ASSERT)
    {
        Token::Ptr tNew(new Token());
        tNew->parent = t;
        tNew->wme = wme;

        tokens_.push_back(tNew);

        for (auto child : children_)
        {
            child->leftActivate(tNew, PropagationFlag::ASSERT);
        }
    }
    else if (flag == PropagationFlag::RETRACT)
    {
        // when the BetaMemory is leftActivated for removal, the WME does not matter!
        // indeed, the previous node might not have done a check at all -- why should it? Checks are
        // expensive, and all we need to do is remove all the tokens that depend on the token we
        // were given for removal
        std::vector<Token::Ptr> toRemove;
        for (auto mt : tokens_)
        {
            if (mt->parent == t) toRemove.push_back(t);
        }

        for (auto mt : toRemove)
        {
            auto it = std::remove(tokens_.begin(), tokens_.end(), mt);
            tokens_.erase(it);

            for (auto child : children_)
            {
                child->leftActivate(mt, PropagationFlag::RETRACT);
            }
        }

    }
}

void BetaMemory::rightRemoval(WME::Ptr wme)
{
    std::vector<Token::Ptr> toRemove;
    for (auto t : tokens_)
    {
        if (t->wme == wme) toRemove.push_back(t);
    }

    for (auto t : toRemove)
    {
        auto it = std::remove(tokens_.begin(), tokens_.end(), t);
        tokens_.erase(it);

        for (auto child : children_)
        {
            child->leftActivate(t, PropagationFlag::RETRACT);
        }
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
