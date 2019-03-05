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
                        bmem_->leftActivate(token, empty, PropagationFlag::ASSERT);
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
            bmem_->rightRemoval(wme);
        }
        return;
    }


    // --- ASSERT ---
    if (isNegative())
    {
        // check the output memory if we need to retract something due to a new match
        std::vector<Token::Ptr> toRetract;
        for (auto token : *bmem_)
        {
            // remember: the token is bmem_ is one element longer, last one is an empty wme.
            if (isValidCombination(token->parent, wme))
            {
                heldBackTokens_[token->parent] = wme;
                toRetract.push_back(token->parent);
            }
        }
        // dont remove (leftActivate with retract) from the bmem_ while iterating it!
        for (auto token : toRetract)
        {
            bmem_->leftActivate(token, nullptr, PropagationFlag::RETRACT);
        }
    }
    else
    {
        // since the wme was added, check for all tokens on the left side if we have a match
        for (auto token : *parentBeta_)
        {
            if (isValidCombination(token, wme))
            {
                bmem_->leftActivate(token, wme, flag);
            }
        }
    }
}

void JoinNode::leftActivate(Token::Ptr token, PropagationFlag flag)
{
    // --- RETRACT ---
    if (flag == PropagationFlag::RETRACT)
    {
        // shortcut: just tell the BetaMemory to remove all these tokens.
        bmem_->leftActivate(token, nullptr, PropagationFlag::RETRACT);
        return;
    }


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
                bmem_->leftActivate(token, wme, flag);
            }
        }
    }

    // if no match was found and the join is negative, propagate the token with an empty wme
    if (isNegative() && !foundMatch)
    {
        EmptyWME::Ptr empty(new EmptyWME());
        empty->isComputed_ = true;
        bmem_->leftActivate(token, empty, PropagationFlag::ASSERT);
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
