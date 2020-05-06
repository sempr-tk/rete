#ifndef RETE_PRODUCTIONNODE_HPP_
#define RETE_PRODUCTIONNODE_HPP_

#include "Production.hpp"
#include "Node.hpp"
#include "Token.hpp"
#include "defs.hpp"
#include "connect.hpp"

namespace rete {
    class BetaMemory;
    typedef std::shared_ptr<BetaMemory> BetaMemoryPtr;

/**
    This class represents the terminal nodes in the rete network. They contain a production that
    represents the consequence and can be executed with a token given by the parent beta memory.

    However, this class is abstract: It does not actually do anything with the arriving tokens and
    the production -- a concrete behaviour is e.g. implemented in the AgendaNode.
*/
class ProductionNode : public Node {
protected:
    BetaMemoryPtr parent_;
    Production::Ptr production_;
    std::string getDOTAttr() const override;
    std::string name_;
    inline void accept(NodeVisitor& visitor) override { visitor.visit(this); }
public:
    using Ptr = std::shared_ptr<ProductionNode>;
    using WPtr = std::weak_ptr<ProductionNode>;
    ProductionNode(Production::Ptr);

    /**
        Initializes the ProductionNode by iterating over the parents contents and calling
        activate on all of them.
    */
    void initialize() override;

    /**
        Set the name of the production node. Used in conjunction with the productions name to form
        the label in the dot-graph.
    */
    void setName(const std::string&);
    std::string getName() const;

    /**
        Connect a ProductionNode to its beta-memory parent
    */
    friend void rete::SetParent(BetaMemoryPtr parent, ProductionNode::Ptr child);

    /**
        Called when a Token is asserted/retracted.
    */
    virtual void activate(Token::Ptr, PropagationFlag) = 0;
};


} /* rete */

#endif /* end of include guard: RETE_PRODUCTIONNODE_HPP_ */
