#ifndef RETE_BETAMEMORY_HPP_
#define RETE_BETAMEMORY_HPP_

#include <vector>
#include <memory>

// #include "BetaNode.hpp"
#include "defs.hpp"
#include "Node.hpp"
#include "Token.hpp"

namespace rete {
    class AlphaMemory;
    typedef std::shared_ptr<AlphaMemory> AlphaMemoryPtr;

    class BetaNode;
    typedef std::shared_ptr<BetaNode> BetaNodePtr;
    typedef std::weak_ptr<BetaNode> BetaNodeWPtr;

    class ProductionNode;
    typedef std::shared_ptr<ProductionNode> ProductionNodePtr;
    typedef std::weak_ptr<ProductionNode> ProductionNodeWPtr;

/**
    The BetaMemory stores the results of a BetaNode and allows the connection to other BetaNodes.
*/
class BetaMemory : public Node {
    std::vector<Token::Ptr> tokens_;
    std::vector<BetaNodeWPtr> children_;
    std::vector<ProductionNodeWPtr> productions_;
    BetaNodePtr parent_;

    std::string getDOTAttr() const override;

    /**
        Adds a child BetaNode that will be left-activated upon changes.
    */
    void addChild(BetaNodePtr);
    void removeChild(BetaNodeWPtr);

    /**
        In principle, ProductionNodes are handled the same way as BetaNodes -- I just made the
        mistake to assume that every BetaNode owns a BetaMemory. So now the ProductionNode class
        does not really match the BetaNode...TODO: I should probably fix this when implementing
        negative nodes?
    */
    void addProduction(ProductionNodePtr);
    void removeProduction(ProductionNodeWPtr);

public:
    using Container = std::vector<Token::Ptr>;
    using Iterator = Container::iterator;

    using Ptr = std::shared_ptr<BetaMemory>;
    using WPtr = std::weak_ptr<BetaMemory>;

    /**
        Connect a BetaMemory to its parent BetaNode
    */
    friend void SetParent(BetaNodePtr parent, BetaMemory::Ptr child);

    // defined in BetaNode, but friend to allow modification of beta memory children.
    friend void SetParents(BetaMemory::Ptr, AlphaMemoryPtr, BetaNodePtr);
    // defined in ProductionNode
    friend void SetParent(BetaMemory::Ptr , ProductionNodePtr);

    /**
        Given a Token t_old and a WME w, adds a new Token t with
            t.parent = t_old
            t.wme = w
        to this BetaMemory, and leftActivates all child-BetaNodes.
        Or, if the PropagationFlag::RETRACT is used instead of PropagationFlag::ASSERT:
        Find all stored tokens that match the given token and wme and retract them.
    */
    void leftActivate(Token::Ptr, WME::Ptr, PropagationFlag);

    /**
        BetaMemories also need to react when a BetaNode is right-activated for removal of an WME!
        Remove all Tokens with token.wme == wme
    */
    void rightRemoval(WME::Ptr);

    void getChildren(std::vector<BetaNodePtr>& children);
    void getProductions(std::vector<ProductionNodePtr>& children);

    size_t size() const;

    Iterator begin();
    Iterator end();
};

} /* rete */

#endif /* end of include guard: RETE_BETAMEMORY_HPP_ */
