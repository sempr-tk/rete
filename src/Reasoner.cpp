#include "../include/Reasoner.hpp"

#include "../include/InferredEvidence.hpp"
#include "../include/AssertedEvidence.hpp"

#include <iostream>

namespace rete {

Network& Reasoner::net()
{
    return rete_;
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

        // what to do?
        // --> find all WMEs that are backed by this evidence (token & production) and remove the corresponding evidence!
        // TODO: Traversing ALL wmes seems a bit heavy, can this be improved in any way? I remember that the reference paper mentioned references at WMEs in which tokens they are used etc, for a tree-based removal (?). Can this be transferred to the reasoner-world, too? Or used for some indexing?
        Evidence::Ptr evidence(new InferredEvidence(token, production));
        for (auto it = backedWMEs_.begin(); it != backedWMEs_.end();)
        {
            it->removeEvidence(evidence);
            if (!it->isBacked())
            {
                std::cout << it->getWME()->toString() << " is no longer backed! FOO" << std::endl;
                rete_.getRoot()->activate(it->getWME(), rete::RETRACT);
                it = backedWMEs_.erase(it);
            } else {
                std::cout << it->getWME()->toString() << " is still backed FOO:" << std::endl;
                for (auto ev : *it)
                {
                    std::cout << "  " << ev->toString() << std::endl;
                }
                ++it;
            }
        }
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
    // announce to rete
    rete_.getRoot()->activate(wme, rete::ASSERT);
}

void Reasoner::removeEvidence(WME::Ptr wme, Evidence::Ptr evidence)
{
    BackedWME nBacked(wme);
    auto it = backedWMEs_.find(nBacked);
    if (it != backedWMEs_.end())
    {
        it->removeEvidence(evidence);
        if (!it->isBacked())
        {
            std::cout << it->getWME()->toString() << " is no longer backed!" << std::endl;

            // lost all evidence --> remove WME!
            rete_.getRoot()->activate(it->getWME(), rete::RETRACT);
            backedWMEs_.erase(it);
        } else {
            std::cout << it->getWME()->toString() << " is still backed:" << std::endl;
            for (auto ev : *it)
            {
                std::cout << "  " << ev->toString() << std::endl;
            }

            std::cout << "But really? Lets check!" << std::endl;
            cleanupInferenceLoops(it->getWME());

            ++it;
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
        // TODO
        std::cout << "FOUND A CIRCLE! " << entryPoint->toString() << " does not hold anymore, as well as: " << std::endl;
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
}

} /* rete */
