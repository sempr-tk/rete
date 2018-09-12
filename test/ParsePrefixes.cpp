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
        Check if prefixes are parsed / accepted at all.
        TODO: Automatically check if the prefixes are used correctly? How? Not worth the trouble, see prefixes.dot.
    */
    bool ok = p.parseRules(
        "@PREFIX rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#>\n"
        "@PREFIX sempr: <sempr:>\n"
        "[(?o rdf:type sempr:SpatialObject), (?o sempr:on ?other) -> (<just> rdf:a <test>)]\n",
        reasoner.net()
    );

    save(reasoner.net(), "prefixes.dot");

    return (ok ? 0 : 1);
}
