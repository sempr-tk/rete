#include "../include/JoinNode.hpp"
#include "../include/AlphaMemory.hpp"


namespace rete {

JoinNode::JoinNode()
{
}

void JoinNode::rightActivate(WME::Ptr wme, PropagationFlag flag)
{
    if (flag == PropagationFlag::RETRACT)
    {
        // no check todo. just pass on to the memory.
        // this is a shortcut so we won't need to scan the parentBeta_ memory again
        bmem_->rightRemoval(wme);
        return;
    }


    // since the wme was added, check for all tokens on the left side if we have a match
    for (auto token : *parentBeta_)
    {
        if (isValidCombination(token, wme))
        {
            bmem_->leftActivate(token, wme, flag);
        }
    }
}

void JoinNode::leftActivate(Token::Ptr token, PropagationFlag flag)
{
    if (flag == PropagationFlag::RETRACT)
    {
        // shortcut: just tell the BetaMemory to remove all these tokens.
        bmem_->leftActivate(token, nullptr, PropagationFlag::RETRACT);
        return;
    }

    // check all WMEs in the alpha memory if they match this token
    for (auto wme : *parentAlpha_)
    {
        if (isValidCombination(token, wme))
        {
            bmem_->leftActivate(token, wme, flag);
        }
    }
}

bool JoinNode::isValidCombination(Token::Ptr, WME::Ptr)
{
    // basic implementation, to be overridden.
    // --> create the full cross-product of beta and alpha memory
    return true;
}

std::string JoinNode::getDOTAttr() const
{
    return "[label=JoinNode]";
}

} /* rete */
