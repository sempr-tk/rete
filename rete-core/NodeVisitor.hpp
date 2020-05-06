#ifndef RETE_NODEVISITOR_HPP_
#define RETE_NODEVISITOR_HPP_

namespace rete {
    class AlphaNode;
    class AlphaMemory;
    class BetaNode;
    class BetaMemory;
    class BetaBetaNode;
    class ProductionNode;

/**
    Interface for any NodeVisitor implementation.
*/
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(AlphaNode*) {}
    virtual void visit(AlphaMemory*) {}
    virtual void visit(BetaNode*) {}
    virtual void visit(BetaMemory*) {}
    virtual void visit(BetaBetaNode*) {}
    virtual void visit(ProductionNode*) {}
};


}

#endif /* include guard: RETE_NODEVISITOR_HPP_ */
