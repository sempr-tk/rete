#ifndef RETE_ALPHANODE_HPP
#define RETE_ALPHANODE_HPP

#include <vector>
#include <memory>

#include "Node.hpp"
#include "WME.hpp"
#include "AlphaMemory.hpp"

namespace rete {


/**
    Alpha nodes make up the first part of the rete network: Every alpha node performs a single
    check on a WME and propagates it forward if it succeeds.
*/
class AlphaNode : public Node {
    std::string getDOTAttr() const override;
    
public:
    using Ptr = std::shared_ptr<AlphaNode>;
    /**
        Child notes will get activated with the results of their parents.
    */
    void addChild(AlphaNode::Ptr);
    void getChildren(std::vector<AlphaNode::Ptr>& children);

    /**
        Activate gets called whenever a new WME arrives and needs to be checked. If the check
        succeeds the implementation of this method must call propagate(wme) in order to propagate
        the wme further to its children.
    */
    virtual void activate(WME::Ptr) = 0;

    /**
        If requested, the AlphaNode may maintain an AlphaMemory.
    */
    bool hasAlphaMemory() const;
    void initAlphaMemory();
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

protected:
    /**
        Calls activate(wme) on all registered child nodes.
    */
    void propagate(WME::Ptr);

private:
    AlphaMemory::Ptr amem_;
    std::vector<AlphaNode::Ptr> children_;
};

} /* rete */


#endif /* end of include guard: RETE_ALPHANODE_HPP */
