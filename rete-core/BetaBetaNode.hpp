#ifndef RETE_BETEBETANODE_HPP_
#define RETE_BETEBETANODE_HPP_

#include <memory>

#include "defs.hpp"
#include "BetaNode.hpp"
#include "BetaMemory.hpp"
#include "connect.hpp"

namespace rete {

class BetaBetaRightActivator;

/**
    This class is a workaround for BetaBeta-Nodes: Nodes that are connected to
    two beta memories.
    Currently I see no other use for this besides the noValue nodes. But to make
    it a bit less hacky lets just implement a generic way to add this kind of
    nodes.

    It is still hacky, as this kind of nodes is not considered in the original
    design of the system. Problems include e.g.:
    - BetaMemory always leftActivate their children, which are BetaNodes
    - The parent of a BetaMemory is supposed to be a BetaNode

    So, how to do this?
    - BetaBetaNode inherits BetaNode. A bit stupid as the rightActivate(wme)
      will never be used, but well, who cares. Same holds for the AlphaBeta-
      Adapters...
    - Add a helper "node": BetaBetaRightActivator. These will
      delegate everything to the BetaBetaNode, map leftActivate(token) to
      BetaBetaNode::rightActivate(token).
    - Since the BetaBetaNode inherits BetaNode it can be easily used as a parent
      for a BetaMemory

*/
class BetaBetaNode : public BetaNode {
    friend class BetaBetaRightActivator;
    BetaNode::Ptr rightActivator_;

    std::string getDOTAttr() const override;

    /**
        Will call leftActivate on all the contents of the left parent memory.
        It is assumed that the node will check the contents of the right parent
        memory on its own for every of these calls.
    */
    void initialize() override;

protected:
    // left and right BetaMemory-parent.
    // parentAlpha_ and parentBeta_ from BetaNode are left unused.
    BetaMemory::Ptr parentRight_, parentLeft_;

public:
    using Ptr = std::shared_ptr<BetaBetaNode>;
    using WPtr = std::weak_ptr<BetaBetaNode>;

    BetaBetaNode();
    BetaNode::Ptr getRightActivator() const;

    /// Connects a BetaBetaNode to its parents
    friend void rete::SetParents(BetaMemory::Ptr parentLeft, BetaMemory::Ptr parentRight, BetaBetaNode::Ptr child);

    /**
        Called upon changes in the left parent memory
    */
    virtual void leftActivate(Token::Ptr, PropagationFlag) override = 0;

    /**
        Called upon changes in the right parent memory
    */
    virtual void rightActivate(Token::Ptr, PropagationFlag) = 0;

    // from BetaNode, unused, just throws an exception
    void rightActivate(WME::Ptr, PropagationFlag) override;

    BetaMemory::Ptr getLeftParent() const;
    BetaMemory::Ptr getRightParent() const;

    bool operator == (const BetaNode& other) const override;
};



/**
    The Helper for rightActivations with tokens from a BetaMemory
*/
class BetaBetaRightActivator : public BetaNode {
    BetaBetaNode* node_; // where to delegate to
public:
    BetaBetaRightActivator(BetaBetaNode* node);

    // ---
    // delegate to the node
    // ---
    std::string getDOTId() const override;
    std::string getDOTAttr() const override;
    void initialize() override;
    void rightActivate(WME::Ptr wme, PropagationFlag flag) override;
    bool operator == (const BetaNode& other) const override;
    BetaMemory::Ptr getBetaMemory() const override;

    // ---
    // this is where the magic happens
    // ---
    void leftActivate(Token::Ptr token, PropagationFlag flag) override;
};


}

#endif /* include guard: RETE_BETEBETANODE_HPP_ */

