#include "BackedWME.hpp"

#include <algorithm>

namespace rete {

BackedWME::BackedWME(WME::Ptr wme)
    : wme_(wme)
{
}

WME::Ptr BackedWME::getWME() const
{
    return wme_;
}

bool BackedWME::isBacked() const
{
    return !evidences_.empty();
}

void BackedWME::addEvidence(Evidence::Ptr e) const
{
    evidences_.insert(e);
}

void BackedWME::removeEvidence(Evidence::Ptr e) const
{
    evidences_.erase(e);
}


BackedWME::Iterator BackedWME::begin() const
{
    return evidences_.begin();
}

BackedWME::Iterator BackedWME::end() const
{
    return evidences_.end();
}

bool BackedWME::SameWME::operator() (const BackedWME& a, const BackedWME& b) const
{
    return *a.wme_ < *b.wme_;
}

} /* rete */
