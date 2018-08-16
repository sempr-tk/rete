#ifndef RETE_PRODUCTIONNODE_HPP_
#define RETE_PRODUCTIONNODE_HPP_

#include "Node.hpp"
#include "Token.hpp"
#include "Production.hpp"
#include "Agenda.hpp"
#include "BetaMemory.hpp"

namespace rete {

/**
    This class represents the terminal nodes in the rete network. They contain a production to
    schedule when the parent beta memory updates a token.
    This node does *not* implement the action to be executed. Its only purpose is to insert /
    remove items to/from the given agenda.
*/
class ProductionNode : public Node {
    Production::Ptr production_;
    Agenda::Ptr agenda_;

    std::string getDOTAttr() const override;
public:
    using Ptr = std::shared_ptr<ProductionNode>;
    ProductionNode(Production::Ptr);

    /**
        Called when a Token is asserted/retracted.
        When a token is asserted, a new pair <token, production> is added to the agenda for
        execution.
        When a token is retracted, a possibly pending item on the agenda is removed.

        NOTE: I'm thinking about an explicit signal to the production, in order handle more complex actions than just the addition/removal of a WME. Therefore it would be possible to check if the action is pending on the agenda -- if not it must have already been executed, so a retract-action would be appropriate. But how much worth is it really? The same result may still be valid due to a match with a slightly different token. Also, this would require to compare tokens by value instead of just the pointers... It's probably better to implement this in an extension / some other module around the rete network.
    */
    void activate(Token::Ptr, PropagationFlag);

    /**
        Connects a ProductionNode with a BetaMemory and an Agenda.
    */
    static void connect(ProductionNode::Ptr, BetaMemory::Ptr, Agenda::Ptr);
};


} /* rete */

#endif /* end of include guard: RETE_PRODUCTIONNODE_HPP_ */
