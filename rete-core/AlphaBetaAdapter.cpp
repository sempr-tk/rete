#include "AlphaBetaAdapter.hpp"


namespace rete {

AlphaBetaAdapter::AlphaBetaAdapter()
{
}

void AlphaBetaAdapter::rightActivate(WME::Ptr wme, PropagationFlag flag)
{
    auto b = bmem_.lock();
    if (b) b->leftActivate(nullptr, wme, flag);
}

void AlphaBetaAdapter::leftActivate(Token::Ptr, PropagationFlag)
{
    // if you run into this exception you've used this class totally wrong...
    // lookup the rete algorithm and how to deal with the first stage of beta nodes!
    throw std::exception();
}

std::string AlphaBetaAdapter::getDOTAttr() const
{
    return "[label=\"AB-Adapter\"]";
}

bool AlphaBetaAdapter::operator== (const BetaNode& o) const
{
    auto other = dynamic_cast<const AlphaBetaAdapter*>(&o);
    return other;
}

} /* rete */
