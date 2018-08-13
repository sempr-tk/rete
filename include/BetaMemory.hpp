#ifndef RETE_BETAMEMORY_HPP_
#define RETE_BETAMEMORY_HPP_

#include <vector>
#include <memory>

// #include "BetaNode.hpp"
#include "Token.hpp"

namespace rete {

    class BetaNode;
    typedef std::shared_ptr<BetaNode> BetaNodePtr;

/**
    The BetaMemory stores the results of a BetaNode and allows the connection to other BetaNodes.
*/
class BetaMemory {
    std::vector<Token::Ptr> tokens_;
    std::vector<BetaNodePtr> children_;
public:
    using Container = std::vector<Token::Ptr>;
    using Iterator = Container::iterator;

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
    void addChild(BetaNodePtr);
    void getChildren(std::vector<BetaNodePtr>& children);


    Iterator begin();
    Iterator end();
};

} /* rete */

#endif /* end of include guard: RETE_BETAMEMORY_HPP_ */
