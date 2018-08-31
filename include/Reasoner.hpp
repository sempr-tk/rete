#ifndef RETE_REASONER_HPP_
#define RETE_REASONER_HPP_

#include <set>

#include "Network.hpp"
#include "BackedWME.hpp"

namespace rete {

/**
    The Reasoner uses a rete network and adds some additional features to it.
    It checks for example if a fact is given through multiple sources / paths in the inference
    chain, and only truly removes WMEs from the network when there is no more evidence for it.

    And I guess it will provide "changelogs" or something similar, a difference in the set of
    inferred facts as a result from adding / removing some from the outside.
*/
class Reasoner {
    std::set<BackedWME, BackedWME::SameWME> backedWMEs_;
    Network rete_;
public:

    /**
        returns a reference to the internal rete network
    */
    Network& net();


    /**
        As long as the Agenda is not empty, takes the first AgendaItem and processes it, adding and
        removing evidence in the process, updating the Agenda. May cause infinite loops if the
        network and productions are ill-formed.
    */
    void performInference();

    /**
        Performs a single step of inference by processing only the first entry in the agenda.
    */
    void performInferenceStep();

    /**
        Adds the evidence for the the WME to the system. If the WME was not yet known, it is
        processed through the Rete network and the internal agenda is updated. Does *not*
        automatically infer knowledge.
    */
    void addEvidence(WME::Ptr wme, Evidence::Ptr evidence);

    /**
        Removed the evidence for the WME. If there are no more evidences for it, the WME is
        retracted from the Rete network, and more inferred WMEs that relied on the WME are removed,
        too. No productions are executed -- this only clears the memories from deprecated
        WMEs/tokens. Due to negative nodes some productions may now have new matches, and are put
        on the agenda.
    */
    void removeEvidence(WME::Ptr, Evidence::Ptr evidence);

    // TODO: provide some kind of changelog

};


} /* rete */


#endif /* end of include guard: RETE_REASONER_HPP_ */
