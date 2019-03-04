#include "NodeBuilder.hpp"

namespace rete {

NodeBuilder::NodeBuilder(const std::string& ctype, BuilderType btype)
    : conditionType_(ctype), builderType_(btype)
{
}

NodeBuilder::~NodeBuilder()
{
}

std::string NodeBuilder::type() const
{
    return conditionType_;
}

NodeBuilder::BuilderType NodeBuilder::builderType() const
{
    return builderType_;
}

void NodeBuilder::buildAlpha(ArgumentList&, std::vector<AlphaNode::Ptr>&) const
{
    throw std::exception(); // not implemented
}

Builtin::Ptr NodeBuilder::buildBuiltin(ArgumentList&) const
{
    throw std::exception(); // not implemented
}

Production::Ptr NodeBuilder::buildEffect(ArgumentList&) const
{
    throw std::exception(); // not implemented
}
} /* rete */
