#ifndef RETE_PRODUCTION_HPP_
#define RETE_PRODUCTION_HPP_

#include <memory>
#include <vector>

#include "Token.hpp"
#include "defs.hpp"
#include "Annotation.hpp"

namespace rete {

/**
    Base class for a production.
*/
class Production {
    const int priority_;
    std::string name_;
public:
    using Ptr = std::shared_ptr<Production>;

    std::vector<Annotation> annotations_; // why hide it...

    /**
        The priority of the ProductionNode influences their order on the agenda.
        The name is only used for visualization purposes.
    */
    Production(int priority = 0, const std::string& name = "");
    virtual ~Production() {};

    /**
        Realize the consequences. This method is called when a match for this production hits the
        top of the agenda.

        The PropagationFlag states if the given token is a new match (ASSERT) or is currently being
        removed (RETRACT). The third argument is a reference to a vector of WMEs which allows the
        production to ASSERT WMEs based on the given token. This is *only* used when the
        PropagationFlag is ASSERT, since every inferred WME needs to track where it came from: From
        which match, and which token. Inferred WMEs that no longer hold due to a removed token are
        handled by the reasoner, and for negations / no-value checks use a corresponding node in
        the Rete network.
    */
    virtual void execute(Token::Ptr, PropagationFlag, std::vector<WME::Ptr>& inferred) = 0;


    int getPriority() const;

    /**
        Get the name of the production for visualization purposes.
    */
    std::string getName() const;
    void setName(const std::string&);

    /**
        Get a string representation of the production for visualization purposes. (More verbose
        than getName())
    */
    virtual std::string toString() const;
};

} /* rete */

#endif /* end of include guard: RETE_PRODUCTION_HPP_ */
