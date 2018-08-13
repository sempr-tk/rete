#include "../include/AlphaBetaAdapter.hpp"


namespace rete {

AlphaBetaAdapter::AlphaBetaAdapter(AlphaMemory::Ptr alpha)
    : BetaNode(nullptr, alpha)
{
}

void AlphaBetaAdapter::rightActivate(WME::Ptr wme)
{
    bmem_->leftActivate(nullptr, wme);
}

void AlphaBetaAdapter::leftActivate(Token::Ptr)
{
    // if you run into this exception you've used this class totally wrong...
    // lookup the rete algorithm and how to deal with the first stage of beta nodes!
    throw std::exception();
}

} /* rete */
