#ifndef RETE_NOVALUE_HPP_
#define RETE_NOVALUE_HPP_

#include <vector>
#include <utility>
#include <algorithm>

#include "Node.hpp"
#include "Util.hpp"
#include "Accessors.hpp"
#include "BetaMemory.hpp"
#include "BetaBetaNode.hpp"

#include <iostream>
namespace rete {

/**
    This class implements a very special type of node: A generic noValue-Node.
    Formally you could call this a beta-beta-Node, since it always connects
    two beta memories.

    It is used to represent complex noValue conditions. To model this rule:
        [c1, noValue{c2, c3, c4} -> ...]
    Just construct conditions for
        [c1, c2, c3, c4]
    and add a noValue node, which gets connected on its left-side to the output
    of c1, and on the right side to the output of c4. The node will let all
    tokens of c1 pass that are not part of any token in c4.

    Be aware that the tokens forwarded by this node have length
        length(c1) + 1
    and not
        length(c4) + 1;
    the "+1" comes from the empty wme appended to keep it consistent with the
    general assumption that every node adds one wme to the token -- just that
    this node adds it to the token of c1 instead of c4.
*/
class NoValue : public BetaBetaNode {
    /**
        The length of the tokens in the left and right parents.
        leftTokenSize_ < rightTokenSize_.
        Used to compute where in the right tokens to check for the left tokens
        presence.
    */
    size_t tokenSizeDiff_;

    // map from the longer token to the shorter (right to left)
    Token::Ptr getCorresponding(Token::Ptr);

public:
    using Ptr = std::shared_ptr<NoValue>();

    /**
        Constructs a new noValue-Node.
        \param diff the size difference between the tokens. How many steps to go
                    back from the right token to get to the left token.
    */
    NoValue(size_t diff);

    std::string getDOTAttr() const override;

    /**
        ASSERT/UPDATE:
        Checks if the token is present in the right memory,
        and if not propagates it.

        RETRACT:
        Just propagates the retract, let the beta memory handle it.
    */
    void leftActivate(Token::Ptr, PropagationFlag) override;

    /**
        ASSERT/UPDATE:
        Checks if any token in the output memory is part of this token, and
        retract it if so.

        RETRACT:
        Checks if any token in the left-memory can now be propagated
    */
    void rightActivate(Token::Ptr, PropagationFlag) override;
};

} /* rete */

#endif /* end of include guard: RETE_NOVALUE_HPP_ */
