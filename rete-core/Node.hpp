#ifndef RETE_NODE_HPP_
#define RETE_NODE_HPP_

#include <string>
#include <memory>

#include "NodeVisitor.hpp"

namespace rete {

/**
    A base class for all nodes including memories, to provide some basic information about them.
    This is introduced to simplify and customize the representation in dot-files (graphviz).
*/
class Node {
    /**
        Initialize this node after adding it to the network. This is used to
        get hold of WMEs that have been added previously and need to be
        processed by this node, too.
    */
    virtual void initialize() = 0;

public:
    using Ptr = std::shared_ptr<Node>;
    /**
        Returns the format description of the node. E.g. "[label=Foo]"
    */
    virtual std::string getDOTAttr() const = 0;

    /**
        Returns the Id which can be used to build edges.
        Default impl returns the pointer address.
    */
    virtual std::string getDOTId() const;

    /**
        Accept a visitor
    */
    virtual void accept(NodeVisitor&) = 0;



    virtual ~Node();
};

} /* rete */


#endif /* end of include guard: RETE_NODE_HPP_ */
