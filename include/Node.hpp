#ifndef RETE_NODE_HPP_
#define RETE_NODE_HPP_

#include <string>
#include <memory>

namespace rete {

/**
    A base class for all nodes including memories, to provide some basic information about them.
    This is introduced to simplify and customize the representation in dot-files (graphviz).
*/
class Node {
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
};

} /* rete */


#endif /* end of include guard: RETE_NODE_HPP_ */
