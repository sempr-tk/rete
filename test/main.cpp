#include <iostream>
#include <fstream>

#include "../include/Network.hpp"
#include "../include/Triple.hpp"
#include "../include/TripleAlpha.hpp"
#include "../include/AlphaMemory.hpp"
#include "../include/JoinNode.hpp"
#include "../include/AlphaBetaAdapter.hpp"
#include "../include/TripleConsistency.hpp"
#include "../include/TripleJoin.hpp"
#include "../include/AgendaNode.hpp"
#include "../include/InferTriple.hpp"

#include "../include/Reasoner.hpp"
#include "../include/AssertedEvidence.hpp"

#include "../include/RuleParser.hpp"

using namespace rete;


void save(Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}

int main()
{
    RuleParser p;
    Reasoner reasoner;

    /**
        Bad rule design! The first two conditions in the second rule are swapped, so a different
        joint node will be created, the sub-network duplicated.
    */
    p.parseRules(
        "[(?a <foo> ?b), (?b <bar> ?c), (?c <baz> ?d) -> (?a <foobaz> ?d)]"
        "[somerandomname42: (?b <bar> ?c), (?a <foo> ?b), (?c <zab> ?d) -> (?a <foozab> ?d)]",
        reasoner.net()
    );

    save(reasoner.net(), "badRules.dot");

    // -----

    RuleParser p2;
    Reasoner reasoner2;

    /**
        Good rule design: The first two conditions match in the two rules, so this subnetwork is
        reused.
    */
    p2.parseRules(
        "[name1:(?a <foo> ?b), (?b <bar> ?c), (?c <baz> ?d) -> (?a <foobaz> ?d)]"
        "[name2:(?a <foo> ?b), (?b <bar> ?c), (?c <zab> ?d) -> (?a <foozab> ?d)]",
        reasoner2.net()
    );

    save(reasoner2.net(), "goodRules.dot");

    return 0;
}
