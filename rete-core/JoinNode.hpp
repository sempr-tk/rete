#ifndef RETE_JOINNODE_HPP_
#define RETE_JOINNODE_HPP_

#include "BetaNode.hpp"

#include <map>

namespace rete {


/**
    A basic JoinNode: For every token in a BetaMemory and WME in an AlphaMemory, performs a given
    check on every combination of Token and WME and, if it succeeds, stores a new Token containing
    both in its BetaMemory.
*/
class JoinNode : public BetaNode {
    std::string getDOTAttr() const override;

    /**
        If a Join is declared negative, it forwards all tokens for which there is *no* matching wme.
        It keeps track of the first matching wme for all retracted tokens. This is a compromise:
        Don't need to check *all* WMEs and create a complete list of reasons to retract a token,
        but also don't need to recheck all retracted tokens once a single WME is retracted (but
        only those who were not retracted because of a different WME).

        - For every new token, it checks if there is a valid wme in the alpha memory. If not, it forwards the token.
        - For every new wme, it checks its *output*-memory - if there is a valid token for this wme, it is retracted.
        - Token-retractions are simply forwarded to the output memory.
        - For every retracted wme, it checks the tokens that are held back because of it again on all other WMEs. If no "isValidCombination" is found, forwards the token.
    */
    bool negative_;
    std::map<Token::Ptr, WME::Ptr> heldBackTokens_;

public:
    JoinNode();
    void rightActivate(WME::Ptr, PropagationFlag) override;
    void leftActivate(Token::Ptr, PropagationFlag) override;

    /**
        Checks if the join is negative.
        Negative joins only forward a token (with an appended empty wme) when there is no matching
        WNE in the connected alpha memory. Effectively implements a "noValue" statement.
    */
    bool isNegative() const;
    void setNegative(bool flag);

    /**
        Check if a token and a wme together fulfill the join condition. Override this method to
        implement the conditions.
    */
    virtual bool isValidCombination(Token::Ptr, WME::Ptr) = 0;
};

} /* rete */


#endif /* end of include guard: RETE_JOINNODE_HPP_ */
