#ifndef RETE_ALPHAMEMORY_HPP_
#define RETE_ALPHAMEMORY_HPP_

#include <unordered_set>
#include <vector>

#include "Node.hpp"
#include "WME.hpp"
#include "BetaNode.hpp"

namespace rete {

/**
    The AlphaMemory is just another node that has no other purpose than to store the WMEs that
    propagated through the alpha-net. Every AlphaMemory stores the WMEs that match a specific set
    of conditions for a _single_ WME. E.g: "subject=Max, predicate=knows".

    The AlphaMemory is created and used by AlphaNodes.
*/
class AlphaMemory : public Node {
    std::unordered_set<WME::Ptr> wmes_;
    std::vector<BetaNode::Ptr> children_;

    std::string getDOTAttr() const override;

protected:
    void propagate(WME::Ptr);

public:
    using Container = std::unordered_set<WME::Ptr>;
    using Iterator = Container::iterator;
    using Ptr = std::shared_ptr<AlphaMemory>;
    size_t size() const;
    void activate(WME::Ptr);

    /**
        Adds a BetaNode to the list of children, which will get right-activated when this
        AlphaMemory is updated.
    */
    void addChild(BetaNode::Ptr);

    /**
        Get the list of children
    */
    void getChildren(std::vector<BetaNode::Ptr>& children);

    Iterator begin();
    Iterator end();
};

} /* rete */


#endif /* end of include guard: RETE_ALPHAMEMORY_HPP_ */
