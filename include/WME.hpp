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
};

} /* rete */


#endif /* end of include guard: RETE_WME_HPP_ */
