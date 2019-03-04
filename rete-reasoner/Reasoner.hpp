#ifndef RETE_REASONER_HPP_
#define RETE_REASONER_HPP_

#include <map>
#include <set>
#include <functional>

#include "../rete-core/Network.hpp"
#include "BackedWME.hpp"
#include "EvidenceComparator.hpp"

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
    std::function<void(WME::Ptr, rete::PropagationFlag)> callback_;

    /**
        Index to easily find all WMEs that are backed by a given evidence.
    */
    std::map<Evidence::Ptr, std::vector<WME::Ptr>, EvidenceComparator> evidenceToWME_;
public:

    /**
        returns a reference to the internal rete network
    */
    Network& net();
    const Network& net() const;


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
        Removes the evidence for the WME. If there are no more evidences for it, the WME is
        retracted from the Rete network, and more inferred WMEs that relied on the WME are removed,
        too. Retraction of tokens are put on the agenda, too.
        Due to negative nodes some productions may now have new matches, and are also put on the
        agenda.
    */
    void removeEvidence(WME::Ptr, Evidence::Ptr evidence);

    /**
        Remove the given evidence from every WME that was backed by it.
        You want to use this method when you remove a complete source for WMEs, e.g. an
        OWL-document with many triples is unloaded, or the internal rete engine notices that a
        previously matching pattern has to be retracted and everything that was backed by it must
        be removed. If you just want to modify a source, say, remove a single triple from a larger
        document, use 'removeEvidence(WME::Ptr, Evidence::Ptr)'.
    */
    void removeEvidence(Evidence::Ptr evidence);

    /**
        Set a function to call when a new value is inferred or removed
    */
    void setCallback(std::function<void(WME::Ptr, rete::PropagationFlag)>);

private:

    /**
        Loops in the inference chain can lead to the case where a fact is inferred through itself,
        and when the original external assertion it is based on is removed it will sustain itself.
        E.g, consider the rule:
            (?a <equiv> ?b) -> (?b <equiv> ?a)
        When asserting T: (<A> <equiv> <B>)
        we infer (<B> <equiv> <A>), and from that (<A> <equiv> <B>) so that the evidences are the
        following:

        (<A> <equiv> <B>) <- asserted, [(<B> <equiv> <A>)]
        (<B> <equiv> <A>) <- [(<A> <equiv> <B>)]

        If we now remove the assertion, both facts still have evidence, but are ultimately in a
        loop without any asserted base fact. To clean this up, the cleanupInferenceLoops does the
        following: The entryPoint is the fact which no longer has any AssertedEvidence. We assume
        that it is no longer valid and go on to check if any InferredEvidence for T would still
        hold. If yes than there are other sources which are backed by asserted facts and everything
        is fine -- if not there are loops which need to be collapsed.

        An InferredEvidence holds if all WMEs in its token still hold; a WME still holds if it has
        either an AssertedEvidence or any of its InferredEvidences still hold (assuming that T does
        *not* hold). All WMEs that do not hold under the assumption that T does not hold are kept
        in a list, and if in the end none of Ts evidences hold anymore, all of those are removed.

        This scan must always be done when removing an evidence, be it an Asserted- or InferredEvidence. E.g.:

        [A] -> [B] -> [C]
                ^------'

        When removing the AssertedEvidence for A we find that it has no other evidences and remove
        it, and in consequence remove the InferredEvidence [A]->[B]. But B still has an evidence,
        namely [C]->[B], -- a circle that could not be found because A does not have a pointer to
        B, only B has one to A. So the check must be done when removing inferred evidences, too.
    */
    void cleanupInferenceLoops(WME::Ptr entryPoint);

    /**
        This method checks if a fact still holds or is only there due to circles in the inference
        chain but not backed by asserted evidence anymore.

        It adds the fact to the notHolding set (assumes it does not hold) and then proceeds to
        check if an evidence for it exists that still holds under this assumption. An evidence for
        A that only holds if A holds is the result of a chain, while those who still hold signal
        that if A would be removed it could still be reconstructed through some rule and this
        evidence.

        - AssertedEvidence always holds.
        - InferredEvidence only holds if all WMEs needed for it still hold -- this is where this
          method recurses to check for connected circles.

        If in the end there is still a valid evidence for the fact, the fact is removed from the
        notHolding set again and 'true' is returned
        If the fact does not hold it stays in the set (so that future checks in different areas of
        the graph don't need to redo this part), and 'false' is returned.

        When the method returns 'false' it does *not* mean that 'notHolding' contains all circles
        involved! It only contains the nodes that are found to not hold so far, but the check
        aborts as soon as a single evidence is found that definitely holds.
    */
    bool checkIfFactHolds(WME::Ptr fact, std::set<WME::Ptr>& notHolding);

    /** Helper for checkIfFactHolds */
    bool checkIfEvidenceHolds(Evidence::Ptr evidence, std::set<WME::Ptr>& notHolding);


    /**
        Forcely removes a WME from the reasoner, despite any evidences that are still left.
        This is used *internally* after some evidence for the fact was removed and the check for
        loops has determined that it is no longer valid, either because no more evidences are
        there, or because all leftover evidences form an inference-circle that is not backed by
        assertions.
    */
    void remove(WME::Ptr fact);
};


} /* rete */


#endif /* end of include guard: RETE_REASONER_HPP_ */
