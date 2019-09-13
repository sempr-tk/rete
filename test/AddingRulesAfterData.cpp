#include <iostream>
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/AssertedEvidence.hpp"
#include "../rete-rdf/ReteRDF.hpp"
#include <vector>

using namespace rete;


bool containsTriple(const std::vector<WME::Ptr>& wmes,
                    const std::string& s, const std::string& p, const std::string& o)
{
    for (auto wme : wmes)
    {
        auto triple = std::dynamic_pointer_cast<Triple>(wme);
        if (triple &&
            triple->subject == s &&
            triple->predicate == p &&
            triple->object == o)
        {
            return true;
        }
    }
    return false;
}



/**
    This test checks if adding rules after adding data works correctly. It therefore repeats adding
    a fact and afterwards a rule that should get a match with the fact (and previously asserted
    ones).
*/
int main()
{
    Reasoner reasoner;
    RuleParser parser;

    int asserts = 0, retracts = 0, updates = 0;
    reasoner.setCallback(
        [&asserts, &retracts, &updates](WME::Ptr, PropagationFlag flag)
        {
            if (flag == PropagationFlag::ASSERT) asserts++;
            if (flag == PropagationFlag::RETRACT) retracts++;
            if (flag == PropagationFlag::UPDATE) updates++;
        }
    );


    auto evidence = std::make_shared<AssertedEvidence>("fact-goup-1");

    // add a fact
    auto t = std::make_shared<Triple>("<a>", "<foo>", "<b>");
    reasoner.addEvidence(t, evidence);

    // and a rule
    parser.parseRules(
        "[rule1: (?a <foo> ?b) -> (<result> <of> <rule1>)]",
        reasoner.net()
    );

    // draw conclusions
    reasoner.performInference();

    // check if it worked
    if (!containsTriple(reasoner.getCurrentState().getWMEs(), "<result>", "<of>", "<rule1>"))
    {
        return 1;
    }

    // check 2:
    t = std::make_shared<Triple>("<a>", "<bar>", "<b>");
    reasoner.addEvidence(t, evidence);
    parser.parseRules(
        "[rule2: (?a <foo> ?b), (?a <bar> ?b) -> (<result> <of> <rule2>)]",
        reasoner.net()
    );
    reasoner.performInference();

    if (!containsTriple(reasoner.getCurrentState().getWMEs(), "<result>", "<of>", "<rule2>"))
    {
        return 2;
    }

    // check 3:
    t = std::make_shared<Triple>("<a>", "<baz>", "<b>");
    reasoner.addEvidence(t, evidence);
    parser.parseRules(
        "[rule3: (?a <foo> ?b), (?a ?p ?b) -> (<rule3> <result> ?p)]",
        reasoner.net()
    );
    reasoner.performInference();

    auto wmes = reasoner.getCurrentState().getWMEs();
    if (!containsTriple(wmes, "<rule3>", "<result>", "<foo>") ||
        !containsTriple(wmes, "<rule3>", "<result>", "<bar>") ||
        !containsTriple(wmes, "<rule3>", "<result>", "<baz>"))
    {
        return 3;
    }


    // check if there were exactly 1+1 + 1+1 + 1+3 = 8 assertions and 0 retracts
    if (asserts != 8 || retracts != 0 || updates != 0)
    {
        return 4;
    }

    return 0;
}
