#ifndef RETE_ALPHANODE_HPP
#define RETE_ALPHANODE_HPP

#include <vector>
#include <memory>

#include "defs.hpp"
#include "Node.hpp"
#include "WME.hpp"
#include "AlphaMemory.hpp"
#include "connect.hpp"

namespace rete {


/**
    Alpha nodes make up the first part of the rete network: Every alpha node performs a single
    check on a WME and propagates it forward if it succeeds.
*/
class AlphaNode : public Node {
    friend class AlphaMemory;
public:
    using Ptr = std::shared_ptr<AlphaNode>;
    using WPtr = std::weak_ptr<AlphaNode>;

private:
    std::string getDOTAttr() const override;

    /**
        Child notes will get activated with the results of their parents.
        Stores only a weak_ptr to the child.
    */
    void addChild(AlphaNode::Ptr);

    /**
        Don't need a full shared_ptr to remove children. Also, there are no shared_ptr for expired
        weak_ptr.
    */
    void removeChild(AlphaNode::WPtr);
    void removeChild(AlphaNode*);

    /**
        Initialize this node. This will look at its parent: If the parent has an alpha-memory,
        it will process all the WMEs in there (again), as if they were just added. If the parent
        does not have an alpha-memory, it will temporally remove all its siblings from the parents
        children-list and call parent->initialize(). That way only the WMEs from the nearest memory
        need to be re-evaluated, and only on the path to this node.

        You should not need to call this method at any time. It is only relevant when a memory is
        newly connected to the node, and you want to initialize the memory node.
    */
    void initialize() override;

    inline void accept(NodeVisitor& visitor) override { visitor.visit(this); }
public:
    /**
        Destructor, removes this node from its parent
    */
    ~AlphaNode();

    /**
        Connects an AlphaNode to its parent and vice versa. Implemented as a static function since
        we need shared/weak ptr for both sides.
    */
    friend void rete::SetParent(AlphaNode::Ptr parent, AlphaNode::Ptr child);
    friend void rete::SetParent(AlphaNode::Ptr parent, AlphaMemory::Ptr child);

    void getChildren(std::vector<AlphaNode::Ptr>& children);

    /**
        Activate gets called whenever a new WME arrives and needs to be checked. If the check
        succeeds the implementation of this method must call propagate(wme) in order to propagate
        the wme further to its children.
        It also needs to differ between the propagation flags ASSERT, RETRACT and UPDATE. ASSERTs
        are described above, RETRACTs can be forwarded and left for the alpha memory to handle it.
        On UPDATE the AlphaNode should re-evaluate the WME, and propagate either UPDATE (on match)
        or RETRACT. The AlphaMemory nodes only propagate RETRACTs when it was actually a previous
        match, and convert an UPDATE into an ASSERT if it wasn't a match before.
    */
    virtual void activate(WME::Ptr, PropagationFlag) = 0;

    /**
        Returns the AlphaMemory of this node, if it is set. Nullptr else.
    */
    AlphaMemory::Ptr getAlphaMemory() const;

    /**
        It must be possible to test two AlphaNodes on equality, in order to reuse existing nodes
        whenever possible. This check does not need to take the complete network structure into
        account: While constructing a network you always start at the root node and add nodes one
        by one, so you can always simply check the list of child nodes of the node you want to
        attach something to.
        See e.g. the TripleAlpha implementation.
    */
    virtual bool operator == (const AlphaNode& other) const = 0;


    std::string toString() const override;

protected:
    /**
        Calls activate(wme) on all registered child nodes.
    */
    void propagate(WME::Ptr, PropagationFlag);

private:
    AlphaMemory::WPtr amem_;
    AlphaNode::Ptr parent_;
    std::vector<AlphaNode::WPtr> children_;
};

} /* rete */


#endif /* end of include guard: RETE_ALPHANODE_HPP */
