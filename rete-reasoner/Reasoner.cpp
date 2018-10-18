#include "Reasoner.hpp"

#include "InferredEvidence.hpp"
#include "AssertedEvidence.hpp"

#include <iostream>
#include <algorithm>

namespace rete {

Network& Reasoner::net()
{
    return rete_;
}

const Network& Reasoner::net() const
{
    return rete_;
}

void Reasoner::setCallback(std::function<void(WME::Ptr, rete::PropagationFlag)> fn)
{
    callback_ = fn;
}

void Reasoner::performInferenceStep()
{
    auto agenda = rete_.getAgenda();
    if (agenda->empty()) return;

    auto item = agenda->front();
    agenda->pop_front();

    Token::Ptr token = std::get<0>(item);
    Production::Ptr production = std::get<1>(item);
    PropagationFlag flag = std::get<2>(item);

    std::vector<WME::Ptr> inferred;
    production->execute(token, flag, inferred);

    if (flag == rete::ASSERT)
    {
        // allow inferred WMEs
        for (auto wme : inferred)
        {
            auto ev = std::make_shared<InferredEvidence>(token, production);
            addEvidence(wme, ev);
        }
    }
    else if (flag == rete::RETRACT)
    {
        // remove this evidence completely, which triggers updates in the rete net and therefore in
        // the agenda, too
        Evidence::Ptr evidence(new InferredEvidence(token, production));
        removeEvidence(evidence);
    }
}

void Reasoner::performInference()
{
    auto agenda = rete_.getAgenda();

    while (!agenda->empty())
    {
        performInferenceStep();
    }
}


void Reasoner::addEvidence(WME::Ptr wme, Evidence::Ptr evidence)
{
    BackedWME nBacked(wme);
    auto p = backedWMEs_.insert(nBacked);
    p.first->addEvidence(evidence); // whether new or old, add the evidence.

    // callback
    if (p.second)
    {
        // its a new WME!
        if(callback_) callback_(wme, rete::ASSERT);
    }

    // remember that the evidence is used to back the WME (indexing)
    evidenceToWME_[evidence].push_back(wme);

    // announce to rete
    rete_.getRoot()->activate(wme, rete::ASSERT);
}

void Reasoner::removeEvidence(Evidence::Ptr evidence)
{
    auto it = evidenceToWME_.find(evidence);
    if (it == evidenceToWME_.end())
    {
        // this evidence backs nothing.
    }
    else
    {
        // completely remove the evidence from the index, but we still need to work with all the
        // WMEs that are backed by it. Removing the entry before iterating shortens the checks in
        // removeEvidence(wme, evidence) when the index is updated.
        std::vector<WME::Ptr> wmes;
        it->second.swap(wmes);
        evidenceToWME_.erase(it);

        for (auto wme : wmes)
        {
            removeEvidence(wme, evidence);
        }
    }
}

void Reasoner::removeEvidence(WME::Ptr wme, Evidence::Ptr evidence)
{
    BackedWME nBacked(wme);
    auto it = backedWMEs_.find(nBacked);
    if (it != backedWMEs_.end())
    {
        it->removeEvidence(evidence);

        // also, update the index:
        auto indexIt = evidenceToWME_.find(evidence);
        if (indexIt != evidenceToWME_.end())
        {
            auto vIt = std::remove_if(indexIt->second.begin(), indexIt->second.end(),
                                        [wme](WME::Ptr w) -> bool
                                        {
                                            return *w == *wme;
                                        }
                                    );
            indexIt->second.erase(vIt, indexIt->second.end());
        }

        // now, can we already remove the WME?
        if (!it->isBacked())
        {
            // lost all evidence --> remove WME!
            rete_.getRoot()->activate(wme, rete::RETRACT);
            if (callback_) callback_(wme, rete::RETRACT);
            backedWMEs_.erase(it);
        } else {
            // the WME seems to be still backed -- but really? check and clean up loops!
            cleanupInferenceLoops(wme);
        }
    }
    else
    {
        // std::cout << wme->toString() << " not found in BackedWMEs" << std::endl;
    }
}



void Reasoner::cleanupInferenceLoops(WME::Ptr entryPoint)
{
    /*
        The reason this method was called is that an evidence for entryPoint has been removed.
        now we need to check if the remaining evidences for it are still grounded in asserted
        evidences, or form an inference-loop.
        If it forms a loop we need to completely remove the WME, and all the evidences it resides
        in, recursivly triggering 'cleanupInferenceLoops' again.
    */
    std::set<WME::Ptr> notHolding;
    bool holds = checkIfFactHolds(entryPoint, notHolding);
    if (!holds)
    {
        std::cout << "FOUND A CIRCLE! These WMEs do not hold anymore:" << std::endl;
        for (auto wme : notHolding)
        {
            std::cout << "  " << wme->toString() << std::endl;
            remove(wme);
        }

    }
}


bool Reasoner::checkIfEvidenceHolds(Evidence::Ptr evidence, std::set<WME::Ptr>& notHolding)
{
    auto asserted = std::dynamic_pointer_cast<AssertedEvidence>(evidence);
    if (asserted) return true;

    auto inferred = std::dynamic_pointer_cast<InferredEvidence>(evidence);
    if (inferred)
    {
        Token::Ptr token = inferred->token();
        while (token)
        {
            if (!checkIfFactHolds(token->wme, notHolding))
            {
                return false;
            }
            token = token->parent;
        }
        // all wme in token hold? --> evidence holds.
        return true;
    }

    // neither asserted nor inferred?!
    return false;
}

bool Reasoner::checkIfFactHolds(WME::Ptr fact, std::set<WME::Ptr>& notHolding)
{
    // no need to check if we already know it does not hold
    if (notHolding.find(fact) != notHolding.end()) return false;

    // assume it does not hold:
    notHolding.insert(fact);

    // check if it even exists
    auto backed = backedWMEs_.find(fact);
    if (backed == backedWMEs_.end()) return false;

    // check every evidence
    for (auto evidence : *backed)
    {
        if (checkIfEvidenceHolds(evidence, notHolding))
        {
            // only need one evidence to hold!
            notHolding.erase(fact);
            return true;
        }
    }

    // none of the evidence held, so the fact does not either.
    return false;
}


void Reasoner::remove(WME::Ptr fact)
{
    BackedWME backed(fact);
    backedWMEs_.erase(backed);

    rete_.getRoot()->activate(fact, rete::RETRACT);

    if (callback_) callback_(fact, rete::RETRACT);
}

} /* rete */
