#ifndef RETE_NETWORK_HPP
#define RETE_NETWORK_HPP

#include <vector>

#include "AlphaNode.hpp"
#include "Agenda.hpp"
#include "ProductionNode.hpp"

namespace rete {

/**
    This class provides an entry point into the network. It maintains a top-level alpha-node where
    the first level of real alpha nodes can be connected to. WMEs added to the network are inserted
    in the top-level node and forward propagated from there.

    It also holds the agenda -- a list of production nodes which need to be activated due to found
    matches.
*/
class Network {
    /**
        Internally, the Network maintains a dummy alpha-node as an entry point. It simply forwards
        everything to the registered child nodes.
    */
    class DummyAlpha : public AlphaNode {
    public:
        using Ptr = std::shared_ptr<DummyAlpha>;
        void activate(WME::Ptr, PropagationFlag) override;
        bool operator == (const AlphaNode& other) const override;
        std::string getDOTAttr() const override;
    };

    /**
        the root node, where the whole network is hinged on. It shall never be deconstructed, hence
        a shared ptr is used here.
    */
    DummyAlpha::Ptr root_;
    AlphaMemory::Ptr holdAlive_; // a shared pointer to hold the amem of root_ alive


    // the net-global agenda for production nodes.
    Agenda::Ptr agenda_;

    /**
        A list of all production nodes that were added to the network. These shared_ptr hold the
        whole network alive, as child-pointer are all weak_ptr!
        When a production node is deconstructed, all nodes in the network that are no longer needed
        will collapse automatically.
    */
    std::vector<ProductionNode::Ptr> productions_;

public:
    Network();

    /**
        On destruction of the Network, all nodes get disconnected.
        Since they are managed by smart pointers, everything should clean itself up.
    */
    ~Network();

    AlphaNode::Ptr getRoot();
    Agenda::Ptr getAgenda();

    // simply stores the node in an internal vector. It will not be connected to the network automatically - how should it even?
    void addProduction(ProductionNode::Ptr);

    /**
        Traverses the graph of nodes and returns it in the dot-format for visualization
    */
    std::string toDot() const;

};

} /* rete */


#endif /* end of include guard: RETE_NETWORK_HPP */
