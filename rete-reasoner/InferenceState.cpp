#include "InferenceState.hpp"

namespace rete {

WMESupportedBy InferenceState::explain(WME::Ptr wme) const
{
    WMESupportedBy support;
    support.wme_ = wme;
    auto it = backedWMEs_.find(wme);
    if (it != backedWMEs_.end())
    {
        support.evidences_.insert(support.evidences_.begin(), it->begin(), it->end());
    }

    return support;
}

EvidenceSupports InferenceState::explainedBy(Evidence::Ptr evidence) const
{
    EvidenceSupports support;
    support.evidence_ = evidence;
    auto it = evidenceToWME_.find(evidence);
    if (it != evidenceToWME_.end())
    {
        support.wmes_.insert(support.wmes_.begin(), it->second.begin(), it->second.end());
    }

    return support;
}

size_t InferenceState::numWMEs() const
{
    return backedWMEs_.size();
}

size_t InferenceState::numEvidences() const
{
    return evidenceToWME_.size();
}


std::vector<WME::Ptr> InferenceState::getWMEs() const
{
    std::vector<WME::Ptr> wmes;
    wmes.reserve(numWMEs());
    for (auto& backed : backedWMEs_)
    {
        wmes.push_back(backed.getWME());
    }

    return wmes;
}




void InferenceState::traverseExplanation(WME::Ptr toExplain, ExplanationVisitor& visitor) const
{
    /*
        In order to allow using WME::Ptrs that are different from the BackedWMEs in the
        inference state, get the actual ptr to a semantically equivalent WME first and use that.
    */
    auto toExplainIt = backedWMEs_.find({toExplain});
    if (toExplainIt == backedWMEs_.end()) return; // its not backed, so we won't even visit the one to explain.


    // breadth first search for correct depth of WMEs (first occurence, closes to the WME toExplain
    // in any chain!).
    typedef std::pair<WME::Ptr, size_t> WMEDepth;
    typedef std::pair<WMESupportedBy, size_t> EvidenceDepth;

    std::vector<WMEDepth> currentWMELayer;
    std::vector<EvidenceDepth> currentEvidenceLayer;

    currentWMELayer.push_back({toExplain, 0});

    std::set<WME::Ptr> explained;
    // as long as there are still WMEs to process (chains always end in evidences!)
    while (!currentWMELayer.empty())
    {
        // process all the WMEs
        for (auto entry : currentWMELayer)
        {
            WME::Ptr wme = entry.first;
            size_t depth = entry.second;

            // prevent circles
            if (explained.find(wme) != explained.end()) continue;
            explained.insert(wme);


            // visit the wme
            visitor.visit(wme, depth);

            // remember to visit the evidences after visiting all wmes in this layer
            auto support = this->explain(wme);
            currentEvidenceLayer.push_back({support, depth+1});
        }

        // wme layer fully processed, reset:
        currentWMELayer.clear();

        // next level: evidences, and record new WMEs to process
        for (auto supportEntry : currentEvidenceLayer)
        {
            auto support = supportEntry.first;
            size_t depth = supportEntry.second;

            for (auto evidence : support.evidences_)
            {
                // differ between asserted and inferred
                auto asserted = std::dynamic_pointer_cast<AssertedEvidence>(evidence);
                if (asserted) visitor.visit(asserted, depth);
                else
                {
                    auto inferred = std::dynamic_pointer_cast<InferredEvidence>(evidence);
                    if (inferred)
                    {
                        visitor.visit(inferred, depth);
                        Token::Ptr token = inferred->token();
                        while (token)
                        {
                            WME::Ptr next = token->wme;
                            currentWMELayer.push_back({next, depth+1});
                            token = token->parent;
                        }
                    }
                    else
                    {
                        // should never happen, only asserted and inferred evidences exist
                        visitor.visit(evidence, depth);
                    }
                }
            }
            // visit the connection. +1 on depth, since the reference evidences are one level deeper
            // in the chain
            visitor.visit(support, depth);
        }

        // evidence layer processed, reset:
        currentEvidenceLayer.clear();
    }
}


} /* rete */
