#ifndef RETE_CONNECT_HPP_
#define RETE_CONNECT_HPP_

#include <memory>


/**
    The "SetParent" / "SetParents" functions do what they sound like, and more:
    They are used to set the parent of a node, or disconnect nodes (nullptr as parent).
    Doing so they will also handle initialization and de-initialization of the nodes. That means
    that upon setting the parent of a memory node, the memory node will initialize itself with
    the results of its parents. When connecting a ProductionNode to a beta memory, it will trigger
    the production for the contents of the memory. When disconnecting a ProductionNode, it will
    trigger RETRACTs for the contents of the memory.

    please NOTE: This implementation does **NOT** cover all the edge cases, permutations of
    connecting and disconnecting nodes, etc. Just create and connect your network top down, and
    disconnect productions if you want. Then everything should be fine.
*/

namespace rete {

    // forward declarations
    class AlphaNode;
    class AlphaMemory;
    class BetaNode;
    class BetaMemory;
    class ProductionNode;


    // connect AlphaNodes and -Memories
    void SetParent(std::shared_ptr<AlphaNode> parent, std::shared_ptr<AlphaNode> child);
    void SetParent(std::shared_ptr<AlphaNode> parent, std::shared_ptr<AlphaMemory> child);

    // connect a BetaNode to a Beta- and AlphaMemory.
    void SetParents(std::shared_ptr<BetaMemory> left, std::shared_ptr<AlphaMemory> right, std::shared_ptr<BetaNode> child);

    // connect a BetaMemory to its parent BetaNode
    void SetParent(std::shared_ptr<BetaNode> parent, std::shared_ptr<BetaMemory> child);

    // connect a ProductionNode to a BetaMemory
    void SetParent(std::shared_ptr<BetaMemory> parent, std::shared_ptr<ProductionNode> child);



} /* rete */


#endif /* end of include guard: RETE_CONNECT_HPP_ */
