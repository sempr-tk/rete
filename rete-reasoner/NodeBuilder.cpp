#include "NodeBuilder.hpp"
#include "Exceptions.hpp"

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
    throw NodeBuilderException("The method buildAlpha has not been implemented!");
}

Builtin::Ptr NodeBuilder::buildBuiltin(ArgumentList&) const
{
    throw NodeBuilderException("The method buildBuiltin has not been implemented!");
}

Production::Ptr NodeBuilder::buildEffect(ArgumentList&) const
{
    throw NodeBuilderException("The method buildEffect has not been implemented!");
}

} /* rete */
