#ifndef RETE_BETAMEMORY_HPP_
#define RETE_BETAMEMORY_HPP_

#include <vector>

// #include "BetaNode.hpp"
#include "Token.hpp"


namespace rete {

    class BetaNode;

/**
    The BetaMemory stores the results of a BetaNode and allows the connection to other BetaNodes.
*/
class BetaMemory {
    std::vector<Token::Ptr> tokens_;
    std::vector<BetaNode::Ptr> children_;
public:
    using Ptr = std::shared_ptr<BetaMemory>;
    /**
        Given a Token t_old and a WME w, adds a new Token t with
            t.parent = t_old
            t.wme = w
        to this BetaMemory, and leftActivates all child-BetaNodes.
    */
    void leftActivate(Token::Ptr, WME::Ptr);

    /**
        Adds a child BetaNode that will be left-activated upon changes.
    */
    void addChild(BetaNode::Ptr);
};

} /* rete */

#endif /* end of include guard: RETE_BETAMEMORY_HPP_ */
