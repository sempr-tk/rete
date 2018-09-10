#include "../include/BetaMemory.hpp"
#include "../include/BetaNode.hpp"
#include "../include/ProductionNode.hpp"
#include "../include/Util.hpp"

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

        for (auto p : productions_)
        {
            p->activate(tNew, PropagationFlag::ASSERT);
        }
    }
    else if (flag == PropagationFlag::RETRACT)
    {
        /*
            When a beta memory is left-activated for removal, different things may be the case:
                1. Only the token is given, and wme is nullptr, because the given token has been removed by the parent node --> remove all token which have this token as a parent
                2. Only the wme is given, because the parent node was an AlphaBetaAdapter. Remove all tokens whose head is the wme
                3. Both token and wme are given --  TODO what to do? delete where (t AND w) or delete where (t OR w)?
                4. None are given -- TODO what to do?
        */
        if ((t && wme) || (!t && !wme)) throw std::exception(); // not implemented. case 3 & 4

        std::vector<Token::Ptr> toRemove;
        for (auto mt : tokens_)
        {
            if ((!t || mt->parent == t) &&  // if t is given it must match
                (!wme || *mt->wme == *wme))   // if wme is given it must match (compare by value!)
            {
                toRemove.push_back(mt);
            }
        }

        for (auto mt : toRemove)
        {
            auto it = std::remove(tokens_.begin(), tokens_.end(), mt);
            tokens_.erase(it);

            for (auto child : children_)
            {
                child->leftActivate(mt, PropagationFlag::RETRACT);
            }

            for (auto p : productions_)
            {
                p->activate(mt, PropagationFlag::RETRACT);
            }
        }

    }
}

void BetaMemory::rightRemoval(WME::Ptr wme)
{
    std::vector<Token::Ptr> toRemove;
    for (auto t : tokens_)
    {
        if (*t->wme == *wme) toRemove.push_back(t);
    }

    for (auto t : toRemove)
    {
        auto it = std::remove(tokens_.begin(), tokens_.end(), t);
        tokens_.erase(it);

        for (auto child : children_)
        {
            child->leftActivate(t, PropagationFlag::RETRACT);
        }

        for (auto p : productions_)
        {
            p->activate(t, PropagationFlag::RETRACT);
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

void BetaMemory::addProduction(ProductionNode::Ptr p)
{
    productions_.push_back(p);
}

void BetaMemory::getProductions(std::vector<ProductionNode::Ptr>& children)
{
    children.reserve(productions_.size());
    for (auto c : productions_) children.push_back(c);
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
        record += "|" + util::dotEscape(t->toString());
    }
    return "[shape=record, label=\"{BetaMemory" + record + "}\"]";
}

void BetaMemory::tearDown()
{
    for (auto child : children_)
    {
        child->tearDown();
    }
    children_.clear();

    for (auto production : productions_)
    {
        production->tearDown();
    }
    productions_.clear();
}

} /* rete */
