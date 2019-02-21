#include <iostream>
#include <fstream>

#include "../rete-rdf/ReteRDF.hpp"
#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-reasoner/AssertedEvidence.hpp"

using namespace rete;


void save(Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}


/**
    Direct cycle-detection test:

    assert --> A  --infer--> B
               ^----infer----'

    // remove assertion

               A  --infer--> B
               ^----infer----'

    // cycle must be detected an torn down.
*/
int main()
{
    RuleParser p;
    Reasoner reasoner;

    p.parseRules(
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
    auto t1 = std::make_shared<Triple>("<A>", "<equivalent>", "<B>");
    reasoner.addEvidence(t1, source);
    reasoner.performInference();

    std::cout << "Diff between asserts and retracts: " << cnt << std::endl;

    reasoner.removeEvidence(source);
    std::cout << "Diff between asserts and retracts: " << cnt << std::endl;

    reasoner.performInference();
    std::cout << "Diff between asserts and retracts: " << cnt << std::endl;

    return cnt;
}
