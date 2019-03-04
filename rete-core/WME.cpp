#include "WME.hpp"


namespace rete {

WME::WME() : isComputed_(false)
{
}

bool WME::operator == (const WME& other) const
{
    return (this == &other) || (!(*this < other) && !(other < *this));
}

bool WME::isComputed() const
{
    return isComputed_;
}

} /* rete */
