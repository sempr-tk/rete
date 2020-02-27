#ifndef RETE_TRUENODE_HPP_
#define RETE_TRUENODE_HPP_

#include "AlphaNode.hpp"
#include "TupleWME.hpp"

namespace rete {

/**
    This AlphaNode does not let any external triple pass, but instead provides
    exactly one EmptyWME itself. This is useful to trigger rules exactly ones
    without any condition. E.g.
        "[true() -> (<foo> <bar> <baz>)]"
    Will simply infer (<foo> <bar> <baz>) from nothing.
    Also interesting for testing new builtins without having to insert data
    manually.
*/
class TrueNode : public AlphaNode {
    std::string getDOTAttr() const override;
    EmptyWME::Ptr true_;
public:
    TrueNode();

    /**
        Will not propagate anything. This node is not interested in any
        kind of WMEs.
    */
    void activate(WME::Ptr, PropagationFlag) override;

    /**
        The default implementation of an AlphaNode is to call its own activate
        on all the WMEs in the parents AlphaMemory, or -- if the parent has no
        AlphaMemory -- to call the parents initialize(). This has to be changed:
        Since this not does not care for WMEs it does not need this initialize()
        behaviour for itself, but only for the AlphaMemory that will get
        connected to it. For that this node will need to provide a WME from
        thin air, to hold even when no WMEs have been added externally.
    */
    void initialize() override;

    /**
        Equality with other nodes is easy -- if the other is a TrueNode, it
        is equal.
    */
    bool operator == (const AlphaNode& other) const override;
};

}

#endif /* include guard: RETE_TRUENODE_HPP_ */
