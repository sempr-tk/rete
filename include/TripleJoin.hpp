#ifndef RETE_TRIPLEJOIN_HPP_
#define RETE_TRIPLEJOIN_HPP_

#include "JoinNode.hpp"
#include "Triple.hpp"

namespace rete {

/**
    This class implements a join node for triples. It must be configured with an index at which
    place in the given token a triple can be found, and which fields shall be checked for equality.
    So, e.g. the pattern:
            C1             C2          C3
        (?a foo ?b), (?c foo ?d), (?d foo ?a)

    Could be implemented as the following network:

    (Alpha C1)         (Alpha C2)       (Alpha C3)
        |                   |               |
        +----(JoinNode)-----+               |
           [unconditional]                  |
                 |                          |
                 +--------(TripleJoin)------+
                        [tokenIndex = 1] --> C1
                        [leftField = SUBJECT] --> C1.subject (?a)]
                        [rightField = OBJECT] --> C3.object (?a)]
                                |
                                |
                              [...]
*/
class TripleJoin : public JoinNode {
    size_t tokenIndex_;
    Triple::Field leftField_, rightField_;

    std::string getDOTAttr() const override;
public:
    using Ptr = std::shared_ptr<TripleJoin>;
    /**
        \param tokenIndex selects a Triple for the check from the token.
        \param left selects the field of the Triple chosen by the tokenIndex from the BetaMemory.
        \param right selects the field of the Triple from the AlphaMemory.
    */
    TripleJoin(size_t tokenIndex, Triple::Field left, Triple::Field right);

    /**
        Checks if the Triple::Field (left) of the i'th WME in the Token (i = tokenIndex) is
        equivalent to the field (right) of the given WME.

        E.g. if the subject of the third WME in the token matches the object of the WME.
        The tokenIndex how far to traverse the parent-pointers of the token.
            0 : token->wme
            1 : token->parent->wme
            ...
    */
    bool isValidCombination(Token::Ptr, WME::Ptr) override;
};

} /* rete */


#endif /* end of include guard: RETE_TRIPLEJOIN_HPP_ */
