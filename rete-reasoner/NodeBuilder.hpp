#ifndef RETE_PARSER_NODE_BUILDER_HPP_
#define RETE_PARSER_NODE_BUILDER_HPP_

#include "Argument.hpp"
#include "../rete-core/AlphaNode.hpp"
#include "../rete-core/Builtin.hpp"
#include "../rete-core/Production.hpp"
#include "NodeBuilderHelper.hpp"

namespace rete {

/*
    The *NodeBuilder interface allows to implement the creation of new node types and register them
    in the RuleParser. There are two different types of NodeBuilders:
    a) alpha-NodeBuilders.
        These are responsible to match an alpha-precondition, a condition that
        only effects the selection of a WME but not a special processing step, and implicitely
        requires a join afterwards. Since the selection of a WME might consist of multiple checks
        in the alpha-layer of the network it returns a vector of AlphaNodes which are to be
        connected together. The connection itself is done by the parser, since it might want to
        reuse some of the existing nodes.

    b) BuiltIn-NodeBuilders.
        These only construct a single BuiltIn-Node, a beta-node, which operates on a token in the
        beta network.

    When implementing one builder type you need to override either buildAlpha or buildBuiltin,
    respectively.
*/

class NodeBuilder {
public:
    enum BuilderType { ALPHA, BUILTIN, EFFECT };
protected:
    NodeBuilder(const std::string& conditionType, BuilderType builderType);
    std::string conditionType_;
    BuilderType builderType_;

public:
    virtual ~NodeBuilder();

    /**
        Returns the identifier for the conditions it handles.
    */
    virtual std::string type() const;

    /**
        Returns the type of this builder, ALPHA or BUILTIN.
    */
    virtual BuilderType builderType() const;

    /**
        Construct a list of necessary nodes to handle the precondition. The type is given
        implicitely through the specific implementation of the NodeBuilder.
    */
    virtual void buildAlpha(ArgumentList& args, std::vector<AlphaNode::Ptr>& nodes) const;

    /**
        Construct a single builtin node from the given arguments. The type is given implicitely
        through the specific implementation of the NodeBuilder.
    */
    virtual Builtin::Ptr buildBuiltin(ArgumentList& args) const;

    /**
    Construct an effect: Productions can be used in AgendaNodes and implement the effects a match.
    Since the AgendaNode implements some management routines and is connected to an actutal agenda
    in a rete network, all this is done by the parser -- and the NodeBuilder in this case does not
    build a Node. Just a Production, which will become part of an AgendaNode. Sorry for that.
    */
    virtual Production::Ptr buildEffect(ArgumentList& args) const;
};

} /* rete */


#endif /* end of include guard: RETE_PARSER_NODE_BUILDER_HPP_ */
