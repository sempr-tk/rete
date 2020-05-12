#include "Builtin.hpp"
#include "Util.hpp"

namespace rete {

Builtin::Builtin(const std::string& name)
    : name_(name)
{
}

Builtin::~Builtin()
{
}

std::string Builtin::toString() const
{
    return name();
}

std::string Builtin::getDOTAttr() const
{
    return "[label=\"" + util::dotEscape(this->name()) + "\"]";
}

std::string Builtin::name() const
{
    return name_;
}

void Builtin::setComputed(WME::Ptr wme, bool flag) const
{
    wme->isComputed_ = flag;
}

void Builtin::rightActivate(WME::Ptr, PropagationFlag)
{
    throw std::exception(); // Builtins are not to be connected with AlphaMemories! They aren't joins, but only computations on sub-matches!
}

void Builtin::leftActivate(Token::Ptr token, PropagationFlag flag)
{
    auto bmem = bmem_.lock();
    if (!bmem) throw std::exception(); // no memory to forward anything to. Should not be possible.

    if (flag == PropagationFlag::RETRACT)
    {
        // shortcut: just tell the BetaMemory to remove all these tokens.
        bmem->leftActivate(token, nullptr, PropagationFlag::RETRACT);
    }
    else if (flag == PropagationFlag::ASSERT)
    {
        // process the submatch
        auto computed = process(token);


        if (computed)
        {
            // mark the WME as a computed one --> does not need evidence to hold inside a token.
            computed->isComputed_ = true;
            bmem->leftActivate(token, computed, flag);
        }
    }
    else if (flag == PropagationFlag::UPDATE)
    {
        // In case of an UPDATE we don't know if it was a match previously, so we need to compute it
        // and explicitely propagate RETRACT, or UPDATE if it matches. The BetaMemory figures out
        // if it actually is an UPDATE, a new match (ASSERT), a no longer match (RETRACT), or if it
        // didnt match before and still doesnt.
        auto computed = process(token);

        if (computed)
        {
            computed->isComputed_ = true;
            bmem->leftActivate(token, computed, PropagationFlag::UPDATE);
        }
        else
        {
            bmem->leftActivate(token, nullptr, PropagationFlag::RETRACT);
        }
    }
}



} /* rete */
