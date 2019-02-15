#ifndef TRIPLE_NODE_BUILDER_HPP_
#define TRIPLE_NODE_BUILDER_HPP_

#include "NodeBuilder.hpp"
#include <rete-rdf/ReteRDF.hpp>

namespace rete {

/**
    A specific implementation of a node builder for Triples.
*/
class TripleNodeBuilder : public NodeBuilder {
public:
    TripleNodeBuilder();
    void buildAlpha(ArgumentList& args, std::vector<AlphaNode::Ptr>& nodes) const override;
};

} /* rete */


#endif /* end of include guard: TRIPLE_NODE_BUILDER_HPP_ */
