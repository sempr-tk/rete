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

    // remember for which nodes a placeholder has already been constructed
    std::set<WME::Ptr> wmeHavingPlaceholder_;
    std::set<Evidence::Ptr> evidenceHavingPlaceholder_;
    // methods to construct at most one placeholder node ("[...]")
    void addPlaceholderFor(WME::Ptr);
    void addPlaceholderFor(Evidence::Ptr);

    // how deep the explanation should be at maximum (just crop everything afterwards)
    size_t maxVizDepth_ = 0;

    void saveRank(WME::Ptr wme, size_t rank)
    {
        if (sameRankWMEs_.size() < rank+1)
        {
            sameRankWMEs_.resize(rank+1);
        }
        sameRankWMEs_[rank].push_back(wme);
    }

    void saveRank(Evidence::Ptr ev, size_t rank)
    {
        if (sameRankEvidences_.size() < rank+1)
        {
            sameRankEvidences_.resize(rank+1);
        }
        sameRankEvidences_[rank].push_back(ev);
    }

public:
    void visit(WMESupportedBy&, size_t depth) override;

    void visit(WME::Ptr, size_t depth) override;
    void visit(Evidence::Ptr, size_t) override {}
    void visit(AssertedEvidence::Ptr, size_t depth) override;
    void visit(InferredEvidence::Ptr, size_t depth) override;

    enum VizSettings {
        NONE = 0,
        FORCE_LOWEST_RANK = 1,      // Inserts nodes and edges to force the nodes to be drawn in the
                                    // order of their appearance, from left to right.
        HIDE_RANK_NODES = 2,        // Hide the nodes added with ENFORCE_LOWEST_RANK.
        EDGE_FORCE_HEAD_EAST = 4,   // Force edges to connect to the right side of the target node
        EDGE_FORCE_TAIL_WEST = 8    // Force edges to connect to the left side of the source node
    };


    /**
        Sets the max depth of an explanation to visualize.
        Must be set before traversing the inference state.
        Depth 0 means no limit..
    */
    void setMaxDepth(size_t depth);
    size_t getMaxDepth() const;

    /**
        Returns created dot string.
    */
    std::string str(VizSettings = NONE) const;

    /** Resets the visitor to reuse it for a different explanation */
    void reset();
};

inline ExplanationToDotVisitor::VizSettings operator | (ExplanationToDotVisitor::VizSettings a, ExplanationToDotVisitor::VizSettings b)
{
    return static_cast<ExplanationToDotVisitor::VizSettings>(static_cast<int>(a) | static_cast<int>(b));
}

} /* rete */


#endif /* end of include guard: RETE_REASONER_EXPLANATION_TO_DOT_VISITOR_HPP_ */
