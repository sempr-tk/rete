#include "TripleTypeAlpha.hpp"
#include "../rete-core/Util.hpp"

namespace rete {

void TripleTypeAlpha::activate(WME::Ptr wme, PropagationFlag flag)
{
    if (flag == PropagationFlag::RETRACT)
    {
        // shortcut: without any check, just propagate the retract.
        propagate(wme, PropagationFlag::RETRACT);
    }
    else
    {
        // UPDATE will never change the type, no special treatment necessary.
        // Just check the type and propagate the flag, or drop it.
        auto t = std::dynamic_pointer_cast<Triple>(wme);
        if (t)
        {
            propagate(wme, flag);
        }
    }
}

bool TripleTypeAlpha::operator==(const AlphaNode& other) const
{
    auto optr = dynamic_cast<const TripleTypeAlpha*>(&other);
    if (optr) return true;
    return false;
}


std::string TripleTypeAlpha::getDOTAttr() const
{
    return "[label=\"Triple\"]";
}


std::string TripleTypeAlpha::toString() const
{
    return "Triple";
}

}
