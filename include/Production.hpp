#ifndef RETE_PRODUCTION_HPP_
#define RETE_PRODUCTION_HPP_

#include <memory>

#include "Token.hpp"

namespace rete {

/**
    Base class for a production.
*/
class Production {
    const int priority_;
public:
    using Ptr = std::shared_ptr<Production>;
    /**
        The priority of the ProductionNode influences their order on the agenda.
    */
    Production(int priority = 0);

    /**
        Realize the consequences. This method is called when a match for this production hits the
        top of the agenda.

        TODO: how to assert and retract WMEs from within this method?
    */
    virtual void execute(Token::Ptr) = 0;


    int getPriority() const;

    /**
        Get the name of the production for visualization purposes.
    */
    virtual std::string getName() const = 0;
};

} /* rete */

#endif /* end of include guard: RETE_PRODUCTION_HPP_ */
