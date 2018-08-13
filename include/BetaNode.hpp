#ifndef RETE_BETA_NODE_HPP_
#define RETE_BETA_NODE_HPP_

#include <memory>

#include "defs.hpp"
#include "Node.hpp"
#include "WME.hpp"
#include "Token.hpp"
#include "BetaMemory.hpp"
// #include "AlphaMemory.hpp"

namespace rete {

    class AlphaMemory;
    typedef std::shared_ptr<AlphaMemory> AlphaMemoryPtr;

/**
    A BetaNode always connects to a BetaMemory (left) and an AlphaMemory (right). It simply joins
    the results of a previously checked chain of conditions (--> Token) with the results of
    "atomic" tests on single WMEs.
*/
class BetaNode : public Node {
    std::string getDOTAttr() const override;
protected:
    AlphaMemoryPtr parentAlpha_;
    BetaMemory::Ptr parentBeta_;
    BetaMemory::Ptr bmem_;
public:
    using Ptr = std::shared_ptr<BetaNode>;
    BetaNode(BetaMemory::Ptr, AlphaMemoryPtr);

    /**
        Called upon changes in the connected AlphaMemory.
    */
    virtual void rightActivate(WME::Ptr, PropagationFlag) = 0;

    /**
        Called upon changes in the connected BetaMemory.
    */
    virtual void leftActivate(Token::Ptr, PropagationFlag) = 0;

    /**
        Get access to the associated beta memory node.
    */
    BetaMemory::Ptr getBetaMemory() const;

};

} /* rete */


#endif /* end of include guard: RETE_BETA_NODE_HPP_ */
