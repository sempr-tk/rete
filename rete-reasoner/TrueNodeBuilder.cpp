#include "TrueNodeBuilder.hpp"
#include "Exceptions.hpp"

namespace rete {

TrueNodeBuilder::TrueNodeBuilder()
    : NodeBuilder("true", BuilderType::ALPHA)
{
}

void TrueNodeBuilder::buildAlpha(ArgumentList& args, std::vector<AlphaNode::Ptr>& nodes) const
{
    if (args.size() != 0)
        throw NodeBuilderException("true() does not take any arguments!");

    auto node = std::make_shared<TrueNode>();
    nodes.push_back(node);
}


}
