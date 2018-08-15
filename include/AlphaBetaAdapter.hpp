#ifndef RETE_ALPHABETAADAPTOR_HPP_
#define RETE_ALPHABETAADAPTOR_HPP_

#include "BetaNode.hpp"
#include "AlphaMemory.hpp"

namespace rete {

/**
    The AlphaBetaAdapter is needed for the first stage of beta nodes, when two AlphaMemories would
    need to be joined together. The adaptor simply makes a connection to one AlphaMemory (but no
    BetaMemory) and transforms als WMEs to tokens.
*/
class AlphaBetaAdapter : public BetaNode {
    std::string getDOTAttr() const override;
public:
    using Ptr = std::shared_ptr<AlphaBetaAdapter>;
    AlphaBetaAdapter();

    /**
        Adds a fresh token to the BetaMemory
    */
    void rightActivate(WME::Ptr, PropagationFlag) override;

    /**
        Throws an std::exception(). The AlphaBetaAdapter should never get left-activated
    */
    void leftActivate(Token::Ptr, PropagationFlag) override;

};

} /* rete */


#endif /* end of include guard: RETE_ALPHABETAADAPTOR_HPP_ */
