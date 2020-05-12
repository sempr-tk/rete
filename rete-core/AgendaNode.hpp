#ifndef RETE_AGENDANODE_HPP_
#define RETE_AGENDANODE_HPP_

#include "Node.hpp"
#include "Token.hpp"
#include "ProductionNode.hpp"
#include "Agenda.hpp"
#include "BetaMemory.hpp"

namespace rete {


class AgendaNode : public ProductionNode {
    Agenda::Ptr agenda_;

    std::string getDOTAttr() const override;
public:
    using Ptr = std::shared_ptr<AgendaNode>;
    AgendaNode(Production::Ptr, Agenda::Ptr);

    /**
        Called when a Token is asserted/retracted.
        When a token is asserted, a new pair <token, production> is added to the agenda for
        execution.
        When a token is retracted, a possibly pending item on the agenda is removed.

        The AgendaNode also triggers a connected Reasoner to remove all evidences which rely on the
        wrapped production and the removed token. TODO

        NOTE: I'm thinking about an explicit signal to the production, in order handle more complex actions than just the addition/removal of a WME. Therefore it must be possible to check if the action is pending on the agenda -- if not it must have already been executed, so a retract-action would be appropriate. But how much worth is it really? The same result may still be valid due to a match with a slightly different token. Also, this would require to compare tokens by value instead of just the pointers... It's probably better to implement this in an extension / some other module around the rete network.
    */
    void activate(Token::Ptr, PropagationFlag) override;

    std::string toString() const override;

};


} /* rete */

#endif /* end of include guard: RETE_AGENDANODE_HPP_ */
