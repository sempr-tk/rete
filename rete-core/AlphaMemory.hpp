#ifndef RETE_ALPHAMEMORY_HPP_
#define RETE_ALPHAMEMORY_HPP_

#include <unordered_set>
#include <set>
#include <vector>

#include "defs.hpp"
#include "Node.hpp"
#include "WME.hpp"
#include "BetaNode.hpp"
#include "WMEComparator.hpp"

namespace rete {

    class AlphaNode;

/**
    The AlphaMemory is just another node that has no other purpose than to store the WMEs that
    propagated through the alpha-net. Every AlphaMemory stores the WMEs that match a specific set
    of conditions for a _single_ WME. E.g: "subject=Max, predicate=knows".

    The AlphaMemory is created and used by AlphaNodes.
*/
class AlphaMemory : public Node {
    std::set<WME::Ptr, WMEComparator> wmes_;
    std::vector<BetaNode::WPtr> children_;
    std::shared_ptr<AlphaNode> parent_;

    std::string getDOTAttr() const override;

    /**
        Adds a BetaNode to the list of children, which will get right-activated when this
        AlphaMemory is updated.
    */
    void addChild(BetaNode::Ptr);

    /**
        Removes a child node.
    */
    void removeChild(BetaNode::WPtr);


protected:
    void propagate(WME::Ptr, PropagationFlag);

public:
    // using Container = std::unordered_set<WME::Ptr>;
    using Container = decltype(wmes_);
    using Iterator = Container::iterator;
    using Ptr = std::shared_ptr<AlphaMemory>;
    using WPtr = std::weak_ptr<AlphaMemory>;

    /**
        Sets the parent node of an AlphaMemory. The parent keeps a weak_ptr to its child,
        the child a shared_ptr to its parent.
    */
    friend void SetParent(std::shared_ptr<AlphaNode> parent, AlphaMemory::Ptr child);

    // defined in BetaNode, but friend to allow modification of alpha memory children.
    friend void SetParents(BetaMemory::Ptr, AlphaMemoryPtr, BetaNodePtr);
    

    size_t size() const;
    void activate(WME::Ptr, PropagationFlag);


    /**
        Get the list of children
    */
    void getChildren(std::vector<BetaNode::Ptr>& children);

    Iterator begin();
    Iterator end();

};

} /* rete */


#endif /* end of include guard: RETE_ALPHAMEMORY_HPP_ */
