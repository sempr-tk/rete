#include "JoinNode.hpp"
#include "AlphaMemory.hpp"
#include "TupleWME.hpp"

namespace rete {

JoinNode::JoinNode()
    : negative_(false)
{
}

void JoinNode::rightActivate(WME::Ptr wme, PropagationFlag flag)
{
    auto bmem = bmem_.lock();
    if (!bmem) throw std::exception(); // should not be possible, as the bmem holds this alive.

    // --- RETRACT ---
    if (flag == PropagationFlag::RETRACT)
    {
        // retracting a wme is a chance to create a new result in a negative join
        if (isNegative())
        {
            // need to check all tokens that have been held back
            for (auto it = heldBackTokens_.begin(); it != heldBackTokens_.end(); /*no incr*/)
            {
                // if it was held back because of a match with this exact wme, check if we can release the token
                if (it->second == wme)
                {
                    // found a token (it->first) that was held back due to this wme
                    // try to find another reason to hold it back
                    bool foundMatch = false;
                    for (auto alpha : *parentAlpha_)
                    {
                        if (isValidCombination(it->first, alpha))
                        {
                            heldBackTokens_[it->first] = alpha;
                            foundMatch = true;
                            break;
                        }
                    }

                    if (foundMatch) it++;
                    else
                    {
                        // no need to hold the token back anymore
                        auto token = it->first;
                        it = heldBackTokens_.erase(it);
                        EmptyWME::Ptr empty(new EmptyWME());
                        empty->isComputed_ = true;
                        bmem->leftActivate(token, empty, PropagationFlag::ASSERT);
                    }
                }
                else
                {
                    it++;
                }
            }
        }
        else
        {
            // no check todo. just pass on to the memory.
            // this is a shortcut so we won't need to scan the parentBeta_ memory again
            bmem->rightRemoval(wme);
        }
    }

    // --- ASSERT ---
    else if (flag == PropagationFlag::ASSERT)
    {
        if (isNegative())
        {
            // check the output memory if we need to retract something due to a new match
            std::vector<Token::Ptr> toRetract;
            for (auto token : *bmem)
            {
                // remember: the token is bmem is one element longer, last one is an empty wme.
                if (isValidCombination(token->parent, wme))
                {
                    heldBackTokens_[token->parent] = wme;
                    toRetract.push_back(token->parent);
                }
            }
            // dont remove (leftActivate with retract) from the bmem while iterating it!
            for (auto token : toRetract)
            {
                bmem->leftActivate(token, nullptr, PropagationFlag::RETRACT);
            }
        }
        else
        {
            // since the wme was added, check for all tokens on the left side if we have a match
            for (auto token : *parentBeta_)
            {
                if (isValidCombination(token, wme))
                {
                    bmem->leftActivate(token, wme, flag);
                }
            }
        }
    }
    else if (flag == PropagationFlag::UPDATE)
    {
        /*
            UPDATEs are interesting: The wme could now match something new, or not match anymore.
            Basically we need to do assert and retract stuff depending on what happened before.

            In the non-negative case it is simple: Go through all tokens, just like in an ASSERT,
            and explicitely propagate UPDATE on match and RETRACT on no-match.

            In the negative case:
            Similar to RETRACT we need to re-check all tokens that have been held back due to the
            updated wme, if it is still the case. Also, we need to check all other tokens if they
            now match the wme, and if so add them to the heldBackTokens_ list. (And RETRACT them,
            of course).
            This can be done in one loop:
            for every token:
                if it is held back:
                    if it is held back by exactly this wme:
                        if it still matches:
                            nothing to do.
                        else:
                            # just like in the RETRACT case
                            if another wme matches the token:
                                save that
                            else:
                                ASSERT the token
                    else:
                        nothing todo, there is another wme blocking this token.
                else:
                    check if the wme now matches and thus blocks the token
                    if it matches:
                        RETRACT the token

            as you see, the UPDATEd wme never leads to an UPDATEd token (in the negative case!),
            since the wme is not part of the "match" (better: "result") -- as there is only a
            result if there is no match!
        */
        if (isNegative())
        {
            // for every token
            for (auto token : *parentBeta_)
            {
                // is it held back?
                auto it = heldBackTokens_.find(token);
                if (it != heldBackTokens_.end())
                {
                    if (it->second == wme)
                    {
                        if (isValidCombination(token, wme))
                        {
                            // nothing to do, still blocking.
                        }
                        else
                        {
                            // just like in retract:
                            // try to find another reason to hold it back
                            bool foundMatch = false;
                            for (auto alpha : *parentAlpha_)
                            {
                                if (isValidCombination(token, alpha))
                                {
                                    heldBackTokens_[token] = alpha;
                                    foundMatch = true;
                                    break;
                                }
                            }
                            // none found? remove the token from the held-back-list and ASSERT it.
                            if (foundMatch)
                            {
                                // nothing to do. already saved the new reason to hold the token
                                // back
                            }
                            else
                            {
                                // no need to hold the token back anymore, ASSERT the token
                                heldBackTokens_.erase(it);
                                EmptyWME::Ptr empty(new EmptyWME());
                                empty->isComputed_ = true;
                                bmem->leftActivate(token, empty, PropagationFlag::ASSERT);
                            }
                        }
                    }
                    else
                    {
                        // nothing to do, token is held back by a different wme
                    }
                }
                else
                {
                    // token is currently not being held back! so check if we have a reason to do
                    // so now:
                    if (isValidCombination(token, wme))
                    {
                        // retract and hold back because of the match
                        heldBackTokens_[token] = wme;
                        bmem->leftActivate(token, nullptr, PropagationFlag::RETRACT);
                    }
                    else
                    {
                        // nope. nothing to do.
                    }
                }
            } // end for all tokens.
        }
        else
        {
            /*
                In the non-negative case it is simple: Go through all tokens, just like in an
                ASSERT, and explicitely propagate UPDATE on match and RETRACT on no-match.
            */
            for (auto token : *parentBeta_)
            {
                if (isValidCombination(token, wme))
                {
                    bmem->leftActivate(token, wme, PropagationFlag::UPDATE);
                }
                else
                {
                    bmem->leftActivate(token, wme, PropagationFlag::RETRACT);
                }
            }
        }
    }
}

void JoinNode::leftActivate(Token::Ptr token, PropagationFlag flag)
{
    auto bmem = bmem_.lock();
    if (!bmem) throw std::exception();

    // --- RETRACT ---
    if (flag == PropagationFlag::RETRACT)
    {
        // shortcut: just tell the BetaMemory to remove all these tokens.
        bmem->leftActivate(token, nullptr, PropagationFlag::RETRACT);
        return;
    }

    else if (flag == PropagationFlag::ASSERT)
    {
        // --- ASSERT ---
        // check all WMEs in the alpha memory if they match this token
        auto foundMatch = false;
        for (auto wme : *parentAlpha_)
        {
            if (isValidCombination(token, wme))
            {
                foundMatch = true;
                if (isNegative())
                {
                    // if a match is found and the join is negative, hold the token back, and remember why.
                    heldBackTokens_[token] = wme;
                    break;
                }
                else
                {
                    bmem->leftActivate(token, wme, flag);
                }
            }
        }

        // if no match was found and the join is negative, propagate the token with an empty wme
        if (isNegative() && !foundMatch)
        {
            EmptyWME::Ptr empty(new EmptyWME());
            empty->isComputed_ = true;
            bmem->leftActivate(token, empty, PropagationFlag::ASSERT);
        }
    }
    else if (flag == PropagationFlag::UPDATE)
    {
        /*
            In the negative case, the token must be checked against the reason it was held back
            again. If that still matches all is good, else we search for another reason. If not
            found we assert the token. If the token wasn't held back before we need to check with
            all wmes if there is a reason now.
        */
        if (isNegative())
        {
            auto it = heldBackTokens_.find(token);
            if (it != heldBackTokens_.end())
            {
                // check again.
                if (isValidCombination(token, it->second))
                {
                    // nothing todo, all good.
                }
                else
                {
                    // search another reason to hold it back.
                    bool foundMatch = false;
                    for (auto alpha : *parentAlpha_)
                    {
                        if (isValidCombination(token, alpha))
                        {
                            heldBackTokens_[token] = alpha;
                            foundMatch = true;
                            break;
                        }
                    }

                    if (foundMatch)
                    {
                        // nothing to do. already saved the new reason to hold the token
                        // back
                    }
                    else
                    {
                        // no need to hold the token back anymore, ASSERT the token
                        heldBackTokens_.erase(it);
                        EmptyWME::Ptr empty(new EmptyWME());
                        empty->isComputed_ = true;
                        bmem->leftActivate(token, empty, PropagationFlag::ASSERT);
                    }
                }
            }
            else
            {
                // not held back before, well, maybe now?
                bool foundMatch = false;
                for (auto alpha : *parentAlpha_)
                {
                    if (isValidCombination(token, alpha))
                    {
                        // yep! RETRACT it!
                        heldBackTokens_[token] = alpha;
                        bmem->leftActivate(token, nullptr, PropagationFlag::RETRACT);
                        break;
                    }
                }

                if (!foundMatch)
                {
                    // no, still don't need to hold it back, so now propagate the UPDATE on the
                    // token.
                    bmem->leftActivate(token, nullptr, PropagationFlag::UPDATE);
                }
            }
        }
        /*
            In the non-negative case, the token must be checked against all wme to see if their
            match must be UPDATEd or RETRACTed. We assume here that all wmes matched before, the
            beta memory will figure this all out.
        */
        else
        {
            for (auto alpha : *parentAlpha_)
            {
                if (isValidCombination(token, alpha))
                {
                    bmem->leftActivate(token, alpha, PropagationFlag::UPDATE);
                }
                else
                {
                    bmem->leftActivate(token, alpha, PropagationFlag::RETRACT);
                }
            }
        }
    }
}

bool JoinNode::isNegative() const
{
    return negative_;
}

void JoinNode::setNegative(bool flag)
{
    negative_ = flag;
}

std::string JoinNode::getDOTAttr() const
{
    return std::string("[label=") + (isNegative() ? "negative " : "") + "JoinNode]";
}

} /* rete */
