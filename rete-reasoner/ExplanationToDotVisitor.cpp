#include "ExplanationToDotVisitor.hpp"
#include "InferenceState.hpp"
#include "../rete-core/Util.hpp"

namespace {
    std::string dotId(void* ptr)
    {
        return "\"" + rete::util::ptrToStr(ptr) + "\"";
    }
}

namespace rete {



void ExplanationToDotVisitor::visit(WME::Ptr wme, size_t depth)
{
    bool isFirst = visitedWMEs_.empty();

    // if we are over max depth, don't process the node
    if (!isFirst && maxVizDepth_ && depth > maxVizDepth_)
    {
        return;
    }

    if (visitedWMEs_.find(wme) == visitedWMEs_.end())
    {
        visitedWMEs_.insert(wme);
        saveRank(wme, depth);

        stream_ << dotId(wme.get()) << "[label=\"" << util::dotEscape(wme->toString()) << "\"";
        if (isFirst)
        {
            stream_ << ", peripheries=2";
        }
        stream_ <<"];\n";
    }
}

void ExplanationToDotVisitor::reset()
{
    stream_.str("");
    visitedWMEs_.clear();
    visitedEvidence_.clear();
    sameRankWMEs_.clear();
    sameRankEvidences_.clear();
    wmeHavingPlaceholder_.clear();
    evidenceHavingPlaceholder_.clear();
    maxVizDepth_ = 0;
}

void ExplanationToDotVisitor::visit(AssertedEvidence::Ptr ev, size_t depth)
{
    // if we are over max depth, don't process the node
    if (maxVizDepth_ && depth > maxVizDepth_) return;

    if (visitedEvidence_.find(ev) == visitedEvidence_.end())
    {
        visitedEvidence_.insert(ev);
        saveRank(ev, depth);

        stream_ << dotId(ev.get()) << "[style=filled, color=lightgrey, shape=diamond, label=\"" << util::dotEscape(ev->toString()) << "\"];\n";
    }
}


void ExplanationToDotVisitor::visit(InferredEvidence::Ptr ev, size_t depth)
{
    // if we are over max depth, don't process the node
    if (maxVizDepth_ && depth > maxVizDepth_) return;

    if (visitedEvidence_.find(ev) == visitedEvidence_.end())
    {
        visitedEvidence_.insert(ev);
        saveRank(ev, depth);

        stream_ << dotId(ev.get()) << "[style=filled, color=lightgrey, shape=polygon, sides=6, label=\"" << util::dotEscape(ev->production()->getName())
                << "\"];\n";

        // which WMEs are needed for this inference?
        auto token = ev->token();
        while (token)
        {
            auto wme = token->wme;

            // only draw the connection if we are below max depth,
            // or if we are at max depth but the wme has already been visited
            if (!maxVizDepth_ || depth < maxVizDepth_ ||
                (depth == maxVizDepth_ && (visitedWMEs_.find(wme) != visitedWMEs_.end())))
            {
                stream_ << dotId(wme.get()) << " -> " << dotId(ev.get()) << ";\n";
            }
            // else, draw a placeholder
            else
            {
                addPlaceholderFor(ev);
            }

            token = token->parent;
        }
    }
}

void ExplanationToDotVisitor::visit(WMESupportedBy& connections, size_t depth)
{
    for (auto ev : connections.evidences_)
    {
        // only draw the connection if we are not over max depth,
        // or if we are at max depth bit the evidence has already been visited
        if (!maxVizDepth_ || depth <= maxVizDepth_)
        {
            stream_ << dotId(ev.get()) << " -> " << dotId(connections.wme_.get()) << ";\n";
        }
        // if only one over max depth, add a placeholder node "[...]"
        else if (depth == maxVizDepth_+1)
        {
            if (visitedEvidence_.find(ev) != visitedEvidence_.end())
            {
                // evidence has already been drawn, so its not on a deeper level, and we can
                // draw the connection without harm
                stream_ << dotId(ev.get()) << " -> " << dotId(connections.wme_.get()) << ";\n";
            }
            else
            {
                // only adds one of none is present so far
                addPlaceholderFor(connections.wme_);
            }
        }
    }
}


void ExplanationToDotVisitor::addPlaceholderFor(WME::Ptr wme)
{
    if (wmeHavingPlaceholder_.find(wme) == wmeHavingPlaceholder_.end())
    {
        wmeHavingPlaceholder_.insert(wme);
        // draw the placeholder node for the wme
        std::string p =  "\"" + rete::util::ptrToStr(wme.get()) + "_placeholder\"";
        stream_ << p << "[label=\"" + rete::util::dotEscape("[...]") + "\"";
        stream_ << ", shape=none";
        stream_ << "];\n";

        // draw the connection
        stream_ << p << " -> " << dotId(wme.get()) << " [constraint=true];\n";
    }
}

void ExplanationToDotVisitor::addPlaceholderFor(Evidence::Ptr ev)
{
    if (evidenceHavingPlaceholder_.find(ev) == evidenceHavingPlaceholder_.end())
    {
        evidenceHavingPlaceholder_.insert(ev);
        // draw the placeholder node for the ev
        std::string p =  "\"" + rete::util::ptrToStr(ev.get()) + "_placeholder\"";
        stream_ << p << "[label=\"" + rete::util::dotEscape("[...]") + "\"";
        stream_ << ", shape=none";
        stream_ << "];\n";

        // draw the connection
        stream_ << p << " -> " << dotId(ev.get()) << " [constraint=true];\n";
    }
}


std::string ExplanationToDotVisitor::str(VizSettings settings) const
{
    std::stringstream ss;
    ss << "digraph {\n rankdir=RL\n";
    ss << "node [fontname=\"Ubuntu Mono\", fontsize=13]\n";

    if (settings & FORCE_LOWEST_RANK) ss << "edge[constraint=false]\n";

    if (settings & EDGE_FORCE_HEAD_EAST) ss << "edge [headport=e];\n";
    if (settings & EDGE_FORCE_TAIL_WEST) ss << "edge [tailport=w];\n";

    ss << stream_.str();

    ss << "\n";

    if (settings & FORCE_LOWEST_RANK)
    {
        size_t maxRank = std::max(sameRankWMEs_.size(), sameRankEvidences_.size());
        if (maxRank > 0)
        {
            for (size_t r = 0; r < maxRank; r++)
            {
                ss << "rank" << r << "[shape=box]";
                if (settings & HIDE_RANK_NODES) ss << " [style=invisible]";
                ss << " [label=\"";
                if (!(settings & HIDE_RANK_NODES)) ss << "level " << r;
                ss << "\"];\n";
            }

            ss << "rank" << (maxRank-1);
            for (size_t r = maxRank-1; r > 0; r--)
            {
                ss << " -> rank" << (r-1);
            }
            ss << "[";
            if (settings & HIDE_RANK_NODES) ss << "style=invis, ";
            ss << "constraint=true";
            ss << "];\n";
        }


        for (size_t r = 0; r < maxRank; r++)
        {
            ss << "{ rank=same; ";
            ss << "rank" << r;
            if (sameRankWMEs_.size() > r)
            {
                for (auto& wme : sameRankWMEs_[r]) ss << " ; " << dotId(wme.get());
            }

            if (sameRankEvidences_.size() > r)
            {
                for (auto& ev : sameRankEvidences_[r]) ss << " ; " << dotId(ev.get());
            }
            ss << ";}";
        }
    }

    ss << "\n}";
    return ss.str();
}

void ExplanationToDotVisitor::setMaxDepth(size_t depth)
{
    maxVizDepth_ = depth;
}

size_t ExplanationToDotVisitor::getMaxDepth() const
{
    return maxVizDepth_;
}


} /* rete */
