#include "../include/JoinNode.hpp"
#include "../include/AlphaMemory.hpp"

namespace rete {

JoinNode::JoinNode(BetaMemory::Ptr beta, AlphaMemoryPtr alpha)
    : BetaNode(beta, alpha)
{
}

void JoinNode::rightActivate(WME::Ptr wme)
{
    // since the wme was added, check for all tokens on the left side if we have a match
    for (auto token : *parentBeta_)
    {
        if (isValidCombination(token, wme))
        {
            bmem_->leftActivate(token, wme);
        }
    }
}

void JoinNode::leftActivate(Token::Ptr token)
{
    // check all WMEs in the alpha memory if they match this token
    for (auto wme : *parentAlpha_)
    {
        if (isValidCombination(token, wme))
        {
            bmem_->leftActivate(token, wme);
        }
    }
}

bool JoinNode::isValidCombination(Token::Ptr, WME::Ptr)
{
    // basic implementation, to be overridden.
    // --> create the full cross-product of beta and alpha memory
    return true;
}

} /* rete */
