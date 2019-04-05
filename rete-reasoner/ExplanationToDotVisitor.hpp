#ifndef RETE_REASONER_EXPLANATION_TO_DOT_VISITOR_HPP_
#define RETE_REASONER_EXPLANATION_TO_DOT_VISITOR_HPP_

#include "ExplanationVisitor.hpp"

#include <sstream>
#include <set>


namespace rete {

/**
    The ExplanationToDotVisitor is an implementation of the ExplanationVisitor that constructs a
    dot representation of the inference graph.
*/

class ExplanationToDotVisitor : public ExplanationVisitor {
    std::stringstream stream_;
    std::set<WME::Ptr> visitedWMEs_;
    std::set<Evidence::Ptr> visitedEvidence_;

    std::vector<std::vector<WME::Ptr>> sameRankWMEs_;
    std::vector<std::vector<Evidence::Ptr>> sameRankEvidences_;

    void saveRank(WME::Ptr wme, int rank)
    {
        if (static_cast<int>(sameRankWMEs_.size()) < rank+1)
        {
            sameRankWMEs_.resize(rank+1);
        }
        sameRankWMEs_[rank].push_back(wme);
    }

    void saveRank(Evidence::Ptr ev, int rank)
    {
        if (static_cast<int>(sameRankEvidences_.size()) < rank+1)
        {
            sameRankEvidences_.resize(rank+1);
        }
        sameRankEvidences_[rank].push_back(ev);
    }

public:
    void visit(WMESupportedBy&, int depth) override;

    void visit(WME::Ptr, int depth) override;
    void visit(Evidence::Ptr, int) override {}
    void visit(AssertedEvidence::Ptr, int depth) override;
    void visit(InferredEvidence::Ptr, int depth) override;

    /**
        Returns created dot string.
        \param enforceRanking inserts invisible nodes and edges to enforce the order of the nodes
               based on the depth at which they first occur in the explanation.
    */
    std::string str(bool enforceRanking = false) const;

    /** Resets the visitor to reuse it for a different explanation */
    void reset();
};

} /* rete */


#endif /* end of include guard: RETE_REASONER_EXPLANATION_TO_DOT_VISITOR_HPP_ */
