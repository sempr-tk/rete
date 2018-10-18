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
    std::string name_;
public:
    using Ptr = std::shared_ptr<ProductionNode>;
    ProductionNode(Production::Ptr);

    /**
        Set the name of the production node. Used in conjunction with the productions name to form
        the label in the dot-graph.
    */
    void setName(const std::string&);
    std::string getName() const;


    /**
        Called when a Token is asserted/retracted.
    */
    virtual void activate(Token::Ptr, PropagationFlag) = 0;

    void tearDown() override;
};


} /* rete */

#endif /* end of include guard: RETE_PRODUCTIONNODE_HPP_ */
