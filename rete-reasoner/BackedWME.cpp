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
    evidences_.push_back(e);
}

void BackedWME::removeEvidence(Evidence::Ptr e) const
{
    auto it = std::remove_if(evidences_.begin(), evidences_.end(),
        [e](Evidence::Ptr o) -> bool {
            return *o == *e;
        }
    );
    evidences_.erase(it, evidences_.end());
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
