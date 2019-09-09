#ifndef RETE_CONNECT_HPP_
#define RETE_CONNECT_HPP_

#include <memory>


// Contains the definitions of the rete::SetParent and rete::SetParents functions.

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
