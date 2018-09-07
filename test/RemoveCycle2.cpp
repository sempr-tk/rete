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


/**
    Supported cycle-detection test:

    assert --> 1 --> 2  --infer--> 3
                     ^----infer----'

    // remove assertion

               2  --infer--> 3
               ^----infer----'

    // cycle must be detected an torn down.
*/
int main()
{
    RuleParser p;
    Reasoner reasoner;

    p.parseRules(
        "[(<A> <B> <C>) -> (<A> <equivalent> <B>)]"
        "[(?a <equivalent> ?b) -> (?b <equivalent> ?a)]",
        reasoner.net()
    );

    int cnt = 0;
    auto counter = [&cnt](WME::Ptr, rete::PropagationFlag flag)
    {
        (flag == rete::ASSERT ? ++cnt : --cnt);
    };

    reasoner.setCallback(counter);

    auto source = std::make_shared<AssertedEvidence>("");
    auto t1 = std::make_shared<Triple>("<A>", "<B>", "<C>");
    reasoner.addEvidence(t1, source);
    reasoner.performInference();

    std::cout << "Diff between asserts and retracts: " << cnt << std::endl;

    reasoner.removeEvidence(source);
    std::cout << "Diff between asserts and retracts: " << cnt << std::endl;

    reasoner.performInference();
    std::cout << "Diff between asserts and retracts: " << cnt << std::endl;


    return cnt;
}
