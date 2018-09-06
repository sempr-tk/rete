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
        "[(?a <equivalent> ?b), (?x <type> ?a) -> (?x <type> ?b)]" \
        "[(?a <equivalent> ?b) -> (?b <equivalent> ?a)]"\
        "[(<A> <equivalent> <B>) -> (<its> <a> <test>)]",
        reasoner.net()
    );



    auto source = std::make_shared<AssertedEvidence>("");
    auto t1 = std::make_shared<Triple>("<A>", "<equivalent>", "<B>");
    reasoner.addEvidence(t1, source);

    save(net, "equiv0.dot");
    reasoner.performInference();
    save(net, "equiv1.dot");
    reasoner.removeEvidence(t1, source);
    reasoner.performInference();
    save(net, "equiv2.dot");

    return 0;
}
