#ifndef RETE_BETACOMPARATOR_HPP_
#define RETE_BETACOMPARATOR_HPP_

#include "BetaNode.hpp"
#include "BetaMemory.hpp"

namespace rete {

/**
    To avoid duplicate tokens in join nodes, the alpha memory must activate its child nodes in a
    certain order: Right-activate descendents before their ancestors. The BetaComparator checks for
    two given BetaNodes if the first is a descendent of the second, and returns true if it is.
*/
class BetaComparator {
public:
    bool operator () (const BetaNode::Ptr& a, const BetaNode::Ptr& b);
};

} /* rete */

#endif /* end of include guard: RETE_BETACOMPARE_HATORP_ */
