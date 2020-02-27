#ifndef TRUENODE_BUILDER_HPP_
#define TRUENODE_BUILDER_HPP_

#include "NodeBuilder.hpp"
#include "../rete-core/TrueNode.hpp"

namespace rete {

/**
    Builder for a TrueNode from "true()" that always holds, and triggers
    exactly once.
*/
class TrueNodeBuilder : public NodeBuilder {
public:
    TrueNodeBuilder();
    void buildAlpha(ArgumentList& args, std::vector<AlphaNode::Ptr>& nodes) const override;
};


}

#endif /* include guard: TRUENODE_BUILDER_HPP_ */
