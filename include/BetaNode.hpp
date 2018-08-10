#ifndef RETE_BETA_NODE_HPP_
#define RETE_BETA_NODE_HPP_

#include <memory>

#include "WME.hpp"
#include "Token.hpp"
#include "BetaMemory.hpp"

namespace rete {

/**
    A BetaNode always connects to a BetaMemory (left) and an AlphaMemory (right). It simply joins
    the results of a previously checked chain of conditions (--> Token) with the results of
    "atomic" tests on single WMEs.
*/
class BetaNode {
    BetaMemory::Ptr bmem_;
public:
    using Ptr = std::shared_ptr<BetaNode>;
    BetaMemory();

    /**
        Called upon changes in the connected AlphaMemory.
    */
    virtual void rightActivate(WME::Ptr) = 0;

    /**
        Called upon changes in the connected BetaMemory.
    */
    virtual void leftActivate(Token::Ptr) = 0;

};

} /* rete */


#endif /* end of include guard: RETE_BETA_NODE_HPP_ */
