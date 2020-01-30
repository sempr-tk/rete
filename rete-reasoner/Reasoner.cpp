#include "Reasoner.hpp"

#include "InferredEvidence.hpp"
#include "AssertedEvidence.hpp"

#include <iostream>
#include <algorithm>
#include <sstream>

namespace rete {

Network& Reasoner::net()
{
    return rete_;
}

const Network& Reasoner::net() const
{
    return rete_;
}

void Reasoner::printHistory() const
{
    std::cout << "Reasoner History -- oldest to youngest" << std::endl;
    for (auto item : history_)
    {
        std::cout << std::get<0>(item)->toString() << " | ";
        std::cout << std::get<1>(item)->toString() << " | ";
        switch(std::get<2>(item))
        {
        case PropagationFlag::ASSERT:
            std::cout << "ASSERT";
            break;
        case PropagationFlag::RETRACT:
            std::cout << "RETRACT";
            break;
        case PropagationFlag::UPDATE:
            std::cout << "UPDATE";
            break;
        }
        std::cout << " | " << std::get<3>(item) << std::endl;
    }
}

InferenceState Reasoner::getCurrentState() const
{
    return state_;
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

    // update history
    history_.push_back(item);
    if (history_.size() > maxHistorySize_) history_.erase(history_.begin());

    agenda->pop_front();

    Token::Ptr token = std::get<0>(item);
    Production::Ptr production = std::get<1>(item);
    PropagationFlag flag = std::get<2>(item);

    std::vector<WME::Ptr> inferred;
    production->execute(token, flag, inferred);

    if (flag == rete::ASSERT)
    {
        // allow inferred WMEs
        auto ev = std::make_shared<InferredEvidence>(token, production);
        for (auto wme : inferred)
        {
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
    else if (flag == rete::UPDATE)
    {
        /*
            "UPDATE" of a match means that something inside has changed.
            The production has already been informed and should have computed new inferred data,
            while the old one has to be retracted.
        */

        // TODO: I'm not sure about the order of processing things here. Remove first, or infer first? If I infer first, an existing and "re-inferred" WME can be kept and just given a new InferredEvidence, while removing first leads to retracting and asserting things again (the callback will be called for both), on the whole inference chain. Because retracts will be added to the agenda, and the incoming asserts will not clear (new tokens). So what to do? Go through all WMEs backed by this evidence, back those which are inferred again with the new evidence (which should be equal to the old one, since it is an UPDATE of an existing token), and retract all that were not re-inferred. Good to have a working WME::operator <  for this now. Beware: The newly inferred WMEs might not be added, since they may be equal to existing ones -- so the existing ones will just get more evidence.


        // // first: retract, to get rid of all data that has only been inferred by this.
        // Evidence::Ptr evidence(new InferredEvidence(token, production));
        // removeEvidence(evidence);
        //
        // // now, assert the newly computed stuff
        // for (auto wme : inferred)
        // {
        //     addEvidence(wme, evidence);
        // }

        Evidence::Ptr evidence(new InferredEvidence(token, production));
        auto previouslyInferred = state_.explainedBy(evidence);
        for (auto wme : previouslyInferred.wmes_)
        {
            bool found = false;
            for (auto newWME = inferred.begin(); newWME != inferred.end(); ++newWME)
            {
                if (*wme == **newWME)
                {
                    // found an equivalent wme among the re-inferred wmes, so all is good and the
                    // evidence can stay.
                    found = true;
                    // also, remove the newWME from the list, as it is not needed anymore.
                    // the old instance will suffice.
                    inferred.erase(newWME);
                    break;
                }
            }

            if (!found)
            {
                // previously inferred WME is not re-inferred, so remove the evidence.
                removeEvidence(wme, evidence);
            }
        }

        // what is left in the inferred-vector are the things that previously were not inferred and have to be added
        for (auto wme : inferred)
        {
            addEvidence(wme, evidence);
        }
    }
}

void Reasoner::performInference(size_t maxSteps)
{
    auto agenda = rete_.getAgenda();

    size_t step = 0;
    while (!agenda->empty())
    {
        performInferenceStep();
        step++;

        if (maxSteps && step > maxSteps)
        {
            std::stringstream ss;
            ss << "performInference exceeded maxSteps (" << maxSteps << ")." << std::endl;
            ss << "The last " << history_.size() << " executed productions where:" << std::endl;
            size_t num = history_.size();
            for (auto& i : history_)
            {
                num--;
                ss << "    "
                        << "[" << num << "]"
                        << (std::get<2>(i) == rete::PropagationFlag::ASSERT ? "[assert]  " : "[retract] ")
                        << std::get<3>(i) << ": "
                        // << std::get<1>(i)->getName() << std::endl;
                        << std::get<0>(i)->toString() << std::endl;
            }

            throw std::runtime_error(ss.str());
        }
    }
}


void Reasoner::addEvidence(WME::Ptr wme, Evidence::Ptr evidence)
{
    BackedWME nBacked(wme);
    auto p = state_.backedWMEs_.insert(nBacked);
    p.first->addEvidence(evidence); // whether new or old, add the evidence.

    // callback
    if (p.second)
    {
        // its a new WME!
        if(callback_) callback_(wme, rete::ASSERT);
    }

    // remember that the evidence is used to back the WME (indexing)
    state_.evidenceToWME_[evidence].push_back(wme);

    // announce to rete
    rete_.getRoot()->activate(wme, rete::ASSERT);
}

void Reasoner::removeEvidence(Evidence::Ptr evidence)
{
    auto it = state_.evidenceToWME_.find(evidence);
    if (it == state_.evidenceToWME_.end())
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
        state_.evidenceToWME_.erase(it);

        for (auto wme : wmes)
        {
            removeEvidence(wme, evidence);
        }
    }
}

void Reasoner::removeEvidence(WME::Ptr wme, Evidence::Ptr evidence)
{
    BackedWME nBacked(wme);
    auto it = state_.backedWMEs_.find(nBacked);
    if (it != state_.backedWMEs_.end())
    {
        it->removeEvidence(evidence);

        // also, update the index:
        auto indexIt = state_.evidenceToWME_.find(evidence);
        if (indexIt != state_.evidenceToWME_.end())
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
            state_.backedWMEs_.erase(it);
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
    // no need to check computations of builtins
    if (fact->isComputed()) return true;

    // no need to check if we already know it does not hold
    if (notHolding.find(fact) != notHolding.end()) return false;

    // assume it does not hold:
    notHolding.insert(fact);

    // check if it even exists
    auto backed = state_.backedWMEs_.find(fact);
    if (backed == state_.backedWMEs_.end()) return false;

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
    state_.backedWMEs_.erase(backed);

    rete_.getRoot()->activate(fact, rete::RETRACT);

    if (callback_) callback_(fact, rete::RETRACT);
}

} /* rete */
