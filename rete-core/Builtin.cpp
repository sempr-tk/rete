#include "Builtin.hpp"

namespace rete {

Builtin::Builtin(const std::string& name)
    : name_(name)
{
}

Builtin::~Builtin()
{
}

std::string Builtin::name() const
{
    return name_;
}

void Builtin::rightActivate(WME::Ptr, PropagationFlag)
{
    throw std::exception(); // Builtins are not to be connected with AlphaMemories! They aren't joins, but only computations on sub-matches!
}

void Builtin::leftActivate(Token::Ptr token, PropagationFlag flag)
{
    if (flag == PropagationFlag::RETRACT)
    {
        // shortcut: just tell the BetaMemory to remove all these tokens.
        bmem_->leftActivate(token, nullptr, PropagationFlag::RETRACT);
    }
    else
    {
        // process the submatch
        auto computed = process(token);

        // mark the WME as a computed one --> does not need evidence to hold inside a token.
        computed->isComputed_ = true;

        if (computed)
        {
            bmem_->leftActivate(token, computed, flag);
        }
    }
}



} /* rete */
