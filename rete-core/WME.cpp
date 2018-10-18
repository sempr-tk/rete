#include "WME.hpp"


namespace rete {

bool WME::operator == (const WME& other) const
{
    return (this == &other) || (!(*this < other) && !(other < *this));
}

} /* rete */
