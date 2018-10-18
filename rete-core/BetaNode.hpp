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
    // BetaNode(BetaMemory::Ptr, AlphaMemoryPtr);
    BetaNode();

    /**
        Connects a BetaNode with the given memories. Lets the BetaNode remember the memories for
        later lookup and also adds the BetaNode as a child of the memories to forward new matches.
    */
    static void connect(BetaNode::Ptr, BetaMemory::Ptr, AlphaMemoryPtr);

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

    /**
        Get the parent alpha memory
    */
    AlphaMemoryPtr getParentAlpha() const;

    /**
        Get the parent beta memory
    */
    BetaMemory::Ptr getParentBeta() const;

    /**
        Compare if two BetaNodes are equivalent. This check is only based on the concrete node and
        does *not* take the network topology into account, because it is to be used when
        constructing the network, before connecting the node in question.
    */
    virtual bool operator == (const BetaNode& other) const = 0;


    void tearDown() override;
};

} /* rete */


#endif /* end of include guard: RETE_BETA_NODE_HPP_ */