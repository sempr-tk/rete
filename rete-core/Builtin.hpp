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
    void rightActivate(WME::Ptr, PropagationFlag) override;
    void leftActivate(Token::Ptr, PropagationFlag) override;
    std::string name_;

    std::string getDOTAttr() const override;

protected:
    /**
        Allows derived builtins to also set the isComputed flag. Necessary if
        it does *not* implement the process(Token) method, but rather overrides
        the leftActivate itself.
    */
    void setComputed(WME::Ptr, bool) const;
public:
    using Ptr = std::shared_ptr<Builtin>;
    Builtin(const std::string& name);
    virtual ~Builtin();

    /**
        Returns the name of the builtin.
    */
    virtual std::string name() const;

    /**
        To be implemented: Process a Token. Returns a WME::Ptr containing the newly created data or a nullpointer if the token should not be forwarded to the next stage.
    */
    virtual WME::Ptr process(Token::Ptr) = 0;

    std::string toString() const override;
};

} /* rete */

#endif /* end of include guard: RETE_BUILTIN_HPP_ */
