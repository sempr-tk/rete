#ifndef RETE_PRODUCTIONNODE_HPP_
#define RETE_PRODUCTIONNODE_HPP_

#include "Production.hpp"
#include "Node.hpp"
#include "Token.hpp"
#include "defs.hpp"

namespace rete {

/**
    This class represents the terminal nodes in the rete network. They contain a production that
    represents the consequence and can be executed with a token given by the parent beta memory.

    However, this class is abstract: It does not actually do anything with the arriving tokens and
    the production -- a concrete behaviour is e.g. implemented in the AgendaNode.
*/
class ProductionNode : public Node {
protected:
    Production::Ptr production_;
    std::string getDOTAttr() const override;
public:
    using Ptr = std::shared_ptr<ProductionNode>;
    ProductionNode(Production::Ptr);

    /**
        Called when a Token is asserted/retracted.
    */
    virtual void activate(Token::Ptr, PropagationFlag) = 0;
};


} /* rete */

#endif /* end of include guard: RETE_PRODUCTIONNODE_HPP_ */
