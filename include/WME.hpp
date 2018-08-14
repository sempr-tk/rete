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
        Check for equality. If we want to remove a WME by value, without knowing the exact instance
        that has been added previously, we need to test if two WMEs represent the same knowledge.

        The base-implementation is an identity check, if (this == &other). Maybe it is better to
        track WMEs in a separate layer outside the actual net to prevent multiple addition? Maybe,
        when adding inferred WMEs to the network, we need to track through which Tokens they were
        inferred, and they can be inferred through multiple? However, even in that case we probably
        need a better comparison than (this == &other) on a pointer level.
    */
    virtual bool operator == (const WME& other) const;
};

} /* rete */


#endif /* end of include guard: RETE_WME_HPP_ */
