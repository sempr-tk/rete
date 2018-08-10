#ifndef RETE_NETWORK_HPP
#define RETE_NETWORK_HPP

#include "AlphaNode.hpp"

namespace rete {

/**
    This class provides an entry point into the network. It maintains a top-level alpha-node where
    the first level of real alpha nodes can be connected to. WMEs added to the network are inserted
    in the top-level node and forward propagated from there.
*/
class Network {
    /**
        Internally, the Network maintains a dummy alpha-node as an entry point. It simply forwards
        everything to the registered child nodes.
    */
    class DummyAlpha : AlphaNode {
    public:
        using Ptr = std::shared_ptr<DummyAlpha>;
        void activate(WME::Ptr) override;
    };

    // the root node, where the whole network is hinged on
    DummyAlpha::Ptr root_;

public:
    Network();
    AlphaNode::Ptr getRoot();
    
};

} /* rete */


#endif /* end of include guard: RETE_NETWORK_HPP */
