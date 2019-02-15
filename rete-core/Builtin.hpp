#ifndef RETE_BUILTIN_HPP_
#define RETE_BUILTIN_HPP_

#include "BetaNode.hpp"

namespace rete {

/**
    The Builtin-class is the base for every custom builtin that is intended to be used on the
    current partial match. It is not a join node, and thus no connection to an alpha-memory is
    needed. Nevertheless it adds exactly one additional WME to the token which contains the
    results of its computation. For this matter you may want to use the TupleWME-class.
*/
class Builtin : public BetaNode {
public:
    std::string getDOTAttr() const override;
    void rightActivate(WME::Ptr, PropagationFlag) override;
    void leftActivate(Token::Ptr, PropagationFlag) override;

    /**
        To be implemented: Return the name of the builtin for visualization purposes
    */
    virtual std::string name() const = 0;

    /**
        To be implemented: Process a Token. Returns a WME::Ptr containing the newly created data or a nullpointer if the token should not be forwarded to the next stage.
    */
    virtual WME::Ptr process(Token::Ptr) = 0;

};

} /* rete */

#endif /* end of include guard: RETE_BUILTIN_HPP_ */
