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
    Network& net = reasoner.net();

    p.parseRules(
        "[(?a <subClassOf> ?b), (?b <subClassOf> ?c) -> (?a <subClassOf> ?c)]" \
        \
        "[(?x <subClassOf> ?y), (?y <subClassOf> ?z),(?z <knows> ?p), (?p <type> <person>)" \
        "  -> (?x <knowsPerson> ?p)]" \
        \
        "[(?someone <type> <person>), (<person> <subClassOf> ?class) -> (?someone <type> ?class)]",
        reasoner.net()
    );


    save(net, "complexExample.dot");

    return 0;
}
