#ifndef RETE_JOINNODE_HPP_
#define RETE_JOINNODE_HPP_

#include "BetaNode.hpp"

namespace rete {


/**
    A basic JoinNode: For every token in a BetaMemory and WME in an AlphaMemory, performs a given
    check on every combination of Token and WME and, if it succeeds, stores a new Token containing
    both in its BetaMemory.
*/
class JoinNode : public BetaNode {
    std::string getDOTAttr() const override;
public:
    JoinNode();
    void rightActivate(WME::Ptr, PropagationFlag) override;
    void leftActivate(Token::Ptr, PropagationFlag) override;

    /**
        Check if a token and a wme together fulfill the join condition. Override this method to
        implement the conditions.
    */
    virtual bool isValidCombination(Token::Ptr, WME::Ptr) = 0;
};

} /* rete */


#endif /* end of include guard: RETE_JOINNODE_HPP_ */
