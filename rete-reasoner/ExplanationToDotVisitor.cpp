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



void ExplanationToDotVisitor::visit(WME::Ptr wme, int depth)
{
    bool isFirst = visitedWMEs_.empty();
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
}

void ExplanationToDotVisitor::visit(AssertedEvidence::Ptr ev, int depth)
{
    if (visitedEvidence_.find(ev) == visitedEvidence_.end())
    {
        visitedEvidence_.insert(ev);
        saveRank(ev, depth);

        stream_ << dotId(ev.get()) << "[style=filled, color=lightgrey, shape=diamond, label=\"" << util::dotEscape(ev->toString()) << "\"];\n";
    }
}


void ExplanationToDotVisitor::visit(InferredEvidence::Ptr ev, int depth)
{
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
            stream_ << dotId(wme.get()) << " -> " << dotId(ev.get()) << ";\n";
            token = token->parent;
        }
    }
}

void ExplanationToDotVisitor::visit(WMESupportedBy& connections, int /*depth*/)
{
    for (auto ev : connections.evidences_)
    {
        stream_ << dotId(ev.get()) << " -> " << dotId(connections.wme_.get()) << ";\n";
    }
}

std::string ExplanationToDotVisitor::str(bool enforceRanking) const
{
    std::stringstream ss;
    ss << "digraph {\n rankdir=RL\n";

    if (enforceRanking) ss << "edge[constraint=false]\n";

    ss << stream_.str();

    ss << "\n";

    if (enforceRanking)
    {
        size_t maxRank = std::max(sameRankWMEs_.size(), sameRankEvidences_.size());
        if (maxRank > 0)
        {
            for (size_t r = 0; r < maxRank; r++)
            {
                ss << "rank" << r << " [style=invisible]" << ";\n";
            }

            ss << "rank" << (maxRank-1);
            for (size_t r = maxRank-1; r > 0; r--)
            {
                ss << " -> rank" << (r-1);
            }
            ss << "[style=invis, constraint=true];\n";
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


} /* rete */
