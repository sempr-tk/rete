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
    friend class BetaMemory;
    std::string getDOTAttr() const override;

    /**
        Initializes this node. Basically a helper for recursive initialization of the child beta
        memory. Iterates over the contents of its parent memory and call left- or rightActivate
        with an ASSERT flag, respectively. If it has both left and right parent (i.e., is a join),
        it only iterates over the left parent.
    */
    void initialize() override;

protected:
    AlphaMemoryPtr parentAlpha_;
    BetaMemory::Ptr parentBeta_;
    BetaMemory::WPtr bmem_;

public:
    using Ptr = std::shared_ptr<BetaNode>;
    using WPtr = std::weak_ptr<BetaNode>;
    BetaNode();

    /**
        Connects a BetaNode with the given memories. Lets the BetaNode remember the memories for
        later lookup and also adds the BetaNode as a child of the memories to forward new matches.
    */
    friend void rete::SetParents(BetaMemory::Ptr parentLeft, AlphaMemoryPtr parentRight, BetaNode::Ptr child);
    friend void rete::SetParent(BetaNode::Ptr parent, BetaMemory::Ptr child);

    /**
        Called upon changes in the connected AlphaMemory.
    */
    virtual void rightActivate(WME::Ptr, PropagationFlag) = 0;

    /**
        Called upon changes in the connected BetaMemory.
    */
    virtual void leftActivate(Token::Ptr, PropagationFlag) = 0;

    /**
        Get access to the associated beta memory node. Returns nullptr of none is set.
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
};

} /* rete */


#endif /* end of include guard: RETE_BETA_NODE_HPP_ */
