#ifndef TRIPLE_CONDITION_BUILDER_HPP_
#define TRIPLE_CONDITION_BUILDER_HPP_

#include "NodeBuilder.hpp"
#include <rete-rdf/ReteRDF.hpp>

namespace rete {

/**
    A specific implementation of a node builder for Triple-conditions.
*/
class TripleConditionBuilder : public NodeBuilder {
public:
    TripleConditionBuilder();
    void buildAlpha(ArgumentList& args, std::vector<AlphaNode::Ptr>& nodes) const override;
};

} /* rete */


#endif /* end of include guard: TRIPLE_CONDITION_BUILDER_HPP_ */
