#include "../include/Reasoner.hpp"

#include "../include/InferredEvidence.hpp"

namespace rete {

Network& Reasoner::net()
{
    return rete_;
}

void Reasoner::performInference()
{
    auto agenda = rete_.getAgenda();

    while (!agenda->empty())
    {
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
            // --> find all WMEs that are backed by this evidence (token & production) and remove the corresponding evidence! TODO
            Evidence::Ptr evidence(new InferredEvidence(token, production));
            for (auto it = backedWMEs_.begin(); it != backedWMEs_.end();)
            {
                it->removeEvidence(evidence);
                if (!it->isBacked())
                {
                    rete_.getRoot()->activate(it->getWME(), rete::RETRACT);
                    it = backedWMEs_.erase(it);
                } else {
                    ++it;
                }
            }
        }
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
            // lost all evidence --> remove WME!
            rete_.getRoot()->activate(it->getWME(), rete::RETRACT);
            backedWMEs_.erase(it);
        }
    }
}


} /* rete */
