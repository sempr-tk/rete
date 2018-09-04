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
    Reasoner r;

    p.parseRules(
        "[(?a <subClassOf> ?b), (?b <subClassOf> ?c) -> (?a <subClassOf> ?c)]" \
        "[(?a <subClassOf> ?b), (?b <subClassOf> ?c), (?a <foo> <bar>) -> (?a <foo> <bar>)]" \
        "[(?x <subClassOf> ?y), (?z <foo> <bar>) -> (<something> <very> <strange>)]",
    r.net());

    Triple::Ptr t1(new Triple("<a>", "<subClassOf>", "<b>"));
    Triple::Ptr t2(new Triple("<b>", "<subClassOf>", "<c>"));
    Triple::Ptr t3(new Triple("<c>", "<foo>", "<bar>"));

    r.addEvidence(t1, AssertedEvidence::Ptr(new AssertedEvidence("")));
    r.addEvidence(t2, AssertedEvidence::Ptr(new AssertedEvidence("")));
    r.addEvidence(t3, AssertedEvidence::Ptr(new AssertedEvidence("")));


    save(r.net(), "parsed.dot");

    return 0;
}
