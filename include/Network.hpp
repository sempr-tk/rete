#ifndef RETE_NETWORK_HPP
#define RETE_NETWORK_HPP

#include "AlphaNode.hpp"
#include "Agenda.hpp"

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
        virtual bool operator == (const AlphaNode& other) const override;
    };

    // the root node, where the whole network is hinged on
    DummyAlpha::Ptr root_;

    Agenda::Ptr agenda_;

public:
    Network();
    AlphaNode::Ptr getRoot();
    Agenda::Ptr getAgenda();

    /**
        Traverses the graph of nodes and returns it in the dot-format for visualization
    */
    std::string toDot() const;

};

} /* rete */


#endif /* end of include guard: RETE_NETWORK_HPP */
