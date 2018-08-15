#ifndef RETE_WME_HPP_
#define RETE_WME_HPP_

#include <memory>

namespace rete {
/**
    The base class for all working memory elements in the rete network.
    These are the facts to be added.
*/
class WME {
public:
    using Ptr = std::shared_ptr<WME>;
    virtual ~WME() {};

    /**
        For visualization only
    */
    virtual std::string toString() const = 0;

    /**
        In order to keep unique sets of WMEs and find identical ones, we need a less-than
        comparison for them.
        Note: Of course no WME can know all other types of WMEs and provide a strict ordering between all of them. But since the first layer of alpha nodes are usually TypeNodes (which are implicit in this implementation), we can assume that the AlphaMemories only contain exactly one type of WME. So if MyWMEType implements the operator < only for other MyWMETypes correctly we should be fine. I think in other cases it should return "1", so that (a < b) && (b < a) shows both the conflict and the inequality.
    */
    virtual bool operator < (const WME& other) const = 0;

    /**
        Check for equality between two WMEs. First checks the memory addresses, then uses the
        operator < to
            return !(a < b) && !(b < a);

    */
    bool operator == (const WME& other) const;
};

} /* rete */


#endif /* end of include guard: RETE_WME_HPP_ */
