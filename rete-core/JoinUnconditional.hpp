#ifndef RETE_JOINUNCONDITIONAL_HPP_
#define RETE_JOINUNCONDITIONAL_HPP_

#include "JoinNode.hpp"

namespace rete {


/**
    A basic JoinUnconditional: Creates the cross-product of the left beta- and the right
    alpha-memory
*/
class JoinUnconditional : public JoinNode {
    std::string getDOTAttr() const override;
public:
        /**
        Check if a token and a wme together fulfill the join condition. Override this method to
        implement the conditions.
    */
    bool isValidCombination(Token::Ptr, WME::Ptr) override;

    bool operator == (const BetaNode&) const override;
};

} /* rete */


#endif /* end of include guard: RETE_JOINUNCONDITIONAL_HPP_ */
