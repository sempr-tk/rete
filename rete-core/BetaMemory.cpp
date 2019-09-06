#include "BetaMemory.hpp"
#include "BetaNode.hpp"
#include "ProductionNode.hpp"
#include "Util.hpp"

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
            auto c = child.lock();
            if (c) c->leftActivate(tNew, PropagationFlag::ASSERT);
        }

        for (auto production : productions_)
        {
            auto p = production.lock();
            if (p) p->activate(tNew, PropagationFlag::ASSERT);
        }
    }
    else if (flag == PropagationFlag::RETRACT)
    {
        /*
            When a beta memory is left-activated for removal, different things may be the case:
                1. Only the token is given, and wme is nullptr, because the given token has been removed by the parent node --> remove all token which have this token as a parent
                2. Only the wme is given, because the parent node was an AlphaBetaAdapter. Remove all tokens whose head is the wme
                3. Both token and wme are given --  This happens when the parent beta node got an UPDATE and re-evaluates all combinations (in case of a join), explicitely retracting not-matching combinations as it does not know if it was a match before. Well, the BetaMemory does know, checks all tokens [*] and removes only those which actually existed before.
                4. None are given -- TODO what to do?

                [*] TODO: Maybe some kind of index would be nice?
        */
        if (!t && !wme) throw std::exception(); // not implemented. case 4

        std::vector<Token::Ptr> toRemove;
        for (auto mt : tokens_)
        {
            if ((!t || mt->parent == t) &&    // if t is given it must match
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
                auto c = child.lock();
                if (c) c->leftActivate(mt, PropagationFlag::RETRACT);
            }

            for (auto production : productions_)
            {
                auto p = production.lock();
                if (p) p->activate(mt, PropagationFlag::RETRACT);
            }
        }
    }
    else if (flag == PropagationFlag::UPDATE)
    {
        /*
            Similar to the RETRACT case: All affected token must be found and propagated with an
            "update" to the children and production nodes.

            Connected beta nodes will re-evaluate on the full join, and send explicit <token, wme>
            pairs which are updated. BuiltIns will evaluate the respective changed token, too, and
            call UPDATE with the newly computed value. This must be recognized here (through the
            isComputed_ flag of the wme) so that we don't try to find the new computation and
            incorrectly ASSERT it as a new match without retracting the old value. Instead, when we
            find an entry with the same token and a computed wme: Replace the wme and propagate an
            UPDATE.
        */
        // long story short: We always expect both token and wme given when getting an UPDATE:
        // EXCEPT: From AlphaBetaAdapters. They only give us a wme, but no token.
        if (!wme) throw std::exception();

        // if the wme is computed we only need to search for the one entry with the given token.
        // note: the edge case where there is no token and only a computed wme given cannot appear,
        // as only AlphaBetaAdapters propagate updates without a token to a beta memory, and they
        // always have a non-computed WME. Right?
        if (wme->isComputed())
        {
            for (auto mt : tokens_)
            {
                if (mt->parent == t)
                {
                    // got it! update the computation, propagate an update.
                    mt->wme = wme;

                    for (auto child : children_)
                    {
                        auto c = child.lock();
                        if (c) c->leftActivate(mt, PropagationFlag::UPDATE);
                    }
                    for (auto production : productions_)
                    {
                        auto p = production.lock();
                        if (p) p->activate(mt, PropagationFlag::UPDATE);
                    }

                    // nothing more to do, only needed to find the first entry.
                    return;
                }
            }

            // couldn't find it, so its new. A new match *because* it changed (and didn't match
            // before). So just process it as an ASSERT.
            leftActivate(t, wme, PropagationFlag::ASSERT);
            return;
        }
        else
        {
            // the wme is not computed, so we actually search for an entry where token and wme match
            for (auto mt : tokens_)
            {
                // TODO: Compare wme by value? Not neccessary, I guess... Since it is an UPDATE the
                // wme *instance* should be known.
                if (mt->parent == t && mt->wme == wme)
                {
                    // got it! propagate an update.
                    for (auto child : children_)
                    {
                        auto c = child.lock();
                        if (c) c->leftActivate(mt, PropagationFlag::UPDATE);
                    }

                    for (auto production : productions_)
                    {
                        auto p = production.lock();
                        if (p) p->activate(mt, PropagationFlag::UPDATE);
                    }

                    // nothing more to do, only this one instance.
                    return;
                }

                // couldn't find it, so its new --> handle it as an ASSERT.
                leftActivate(t, wme, PropagationFlag::ASSERT);
                return;
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
            auto c = child.lock();
            if (c) c->leftActivate(t, PropagationFlag::RETRACT);
        }

        for (auto production : productions_)
        {
            auto p = production.lock();
            if (p) p->activate(t, PropagationFlag::RETRACT);
        }
    }

}

void SetParent(BetaNode::Ptr parent, BetaMemory::Ptr child)
{
    if (parent->bmem_.lock()) throw std::exception(); // new parent already has a bmem!

    if (child->parent_) child->parent_->bmem_.reset();
    child->parent_ = parent;
    parent->bmem_ = child;
}

void BetaMemory::addChild(BetaNode::Ptr node)
{
    children_.push_back(node);
}

void BetaMemory::removeChild(BetaNode::WPtr child)
{
    children_.erase(
        std::find_if(children_.begin(), children_.end(), util::EqualWeak<BetaNode>(child)),
        children_.end()
    );
}

void BetaMemory::getChildren(std::vector<BetaNode::Ptr>& children)
{
    children.reserve(children_.size());
    for (auto child : children_)
    {
        auto c = child.lock();
        if (c) children.push_back(c);
    }
}

void BetaMemory::addProduction(ProductionNode::Ptr p)
{
    productions_.push_back(p);
}

void BetaMemory::removeProduction(ProductionNode::WPtr p)
{
    productions_.erase(
        std::remove_if(productions_.begin(), productions_.end(), util::EqualWeak<ProductionNode>(p)),
        productions_.end()
    );
}

void BetaMemory::getProductions(std::vector<ProductionNode::Ptr>& children)
{
    children.reserve(productions_.size());
    for (auto production : productions_)
    {
        auto p = production.lock();
        if (p) children.push_back(p);
    }
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

} /* rete */
