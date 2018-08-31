#include "../include/Reasoner.hpp"

#include "../include/InferredEvidence.hpp"

// #include <iostream>

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
                // std::cout << it->getWME()->toString() << " is no longer backed!" << std::endl;
                rete_.getRoot()->activate(it->getWME(), rete::RETRACT);
                it = backedWMEs_.erase(it);
            } else {
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
            // std::cout << it->getWME()->toString() << " is no longer backed!" << std::endl;

            // lost all evidence --> remove WME!
            rete_.getRoot()->activate(it->getWME(), rete::RETRACT);
            backedWMEs_.erase(it);
        }
    }
    else
    {
        // std::cout << wme->toString() << " not found in BackedWMEs" << std::endl;
    }
}


} /* rete */
