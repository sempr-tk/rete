#ifndef RETE_TRIPLETYPEALPHA_HPP_
#define RETE_TRIPLETYPEALPHA_HPP_

#include "../rete-core/AlphaNode.hpp"
#include "Triple.hpp"

namespace rete {

/**
    An AlphaNode that only checks if a WME is of type rete::Triple.
*/
class TripleTypeAlpha : public AlphaNode {
    std::string getDOTAttr() const override;
public:
    void activate(WME::Ptr, PropagationFlag) override;
    bool operator == (const AlphaNode& other) const override;

    std::string toString() const override;
};

}

#endif /* include guard: RETE_TRIPLETYPEALPHA_HPP_ */
