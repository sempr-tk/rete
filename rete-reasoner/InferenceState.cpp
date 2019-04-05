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
    explained_.clear();

    /*
        In order to allow using WME::Ptrs that are different from the BackedWMEs in the
        inference state, get the actual ptr to a semantically equivalent WME first and use that.
    */
    auto toExplainIt = backedWMEs_.find({toExplain});
    if (toExplainIt == backedWMEs_.end()) return; // its not backed, so we won't even visit the one to explain.

    traverse(toExplainIt->getWME(), visitor, 0);
}

void InferenceState::traverse(WME::Ptr toExplain, ExplanationVisitor& visitor, int depth) const
{
    if (explained_.find(toExplain) != explained_.end()) return;

    explained_.insert(toExplain);

    auto support = this->explain(toExplain);
    if (!support.evidences_.empty())
    {
        // first visit all the nodes
        visitor.visit(toExplain, depth);
        for (auto evidence : support.evidences_)
        {
            traverse(evidence, visitor, depth+1);
        }
        // visit the connections between WME and evidence after visiting the both
        visitor.visit(support, depth);
    }

}

void InferenceState::traverse(Evidence::Ptr evidence, ExplanationVisitor& visitor, int depth) const
{
    //visitor.visit(evidence)
    // sadly, need to cast.
    // the correct way would be to give the Evidence an accept(visitor) method to do the dispatching
    // but since all the datastructures are using shared_ptr, getting a raw Evidence* doesn't really
    // help the visitor. (Needs to find it in sets of Evidence::Ptr etc.)
    auto asserted = std::dynamic_pointer_cast<AssertedEvidence>(evidence);
    if (asserted)
    {
        visitor.visit(asserted, depth);
    }
    else
    {
        auto inferred = std::dynamic_pointer_cast<InferredEvidence>(evidence);
        if (inferred)
        {
            visitor.visit(inferred, depth);
            auto token = inferred->token();
            while (token)
            {
                traverse(token->wme, visitor, depth+1);
                token = token->parent;
            }
        }
        else
        {
            // ?! I didn't implement anything else besides inferred and asserted evidences...
            visitor.visit(evidence, depth);
        }
    }
}


} /* rete */
