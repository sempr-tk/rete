#include "../include/AlphaMemory.hpp"

#include <iostream> // debug

namespace rete {

void AlphaMemory::activate(WME::Ptr wme)
{
    std::cout << "AlphaMemory " << this << " activated: " << wme.get() << std::endl;
    // TODO: check for duplicates? be aware: currently only ptr-comparison in set!
    wmes_.insert(wme);
    propagate(wme);
}

// bool AlphaMemory::operator == (const AlphaNode& other) const
// {
//     // if other is an AlphaMemory, the node is equivalent.
//     // there should always be 1 alphamemory-child maximum.
//     return (nullptr != dynamic_cast<const AlphaMemory*>(&other));
// }


size_t AlphaMemory::size() const
{
    return wmes_.size();
}

} /* rete */
