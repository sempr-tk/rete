#include <iostream>
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/AssertedEvidence.hpp"
#include "../rete-rdf/ReteRDF.hpp"
#include <vector>
#include <fstream>

using namespace rete;

void save(Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}


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
    This test checks if removing rules is handled correctly, i.e. the rule activations are retracted
    and the inferred knowledge is removed. It therefore removes rules one by one and checks if the
    inferred triples are still present.
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

    // add some rules
    auto rules = parser.parseRules(
        "[rule1: (?a <foo> ?b) -> (<result> <of> <rule1>)]"
        "[rule2a: (?a <bar> ?b) -> (<result> <of> <rule2a>)]"
        "[rule2b: (?a <of> <rule2a>) -> (?a <of> <rule2b>)]"
        ,
        reasoner.net()
    );


    auto evidence = std::make_shared<AssertedEvidence>("fact-goup-1");

    // add two facts to trigger all rules
    auto t = std::make_shared<Triple>("<a>", "<foo>", "<b>");
    reasoner.addEvidence(t, evidence);
    t = std::make_shared<Triple>("<a>", "<bar>", "<b>");
    reasoner.addEvidence(t, evidence);

    // draw conclusions
    reasoner.performInference();

    save(reasoner.net(), "RemovingRules_1.dot");

    // check if the results are there before checking if they get removed
    if (!containsTriple(reasoner.getCurrentState().getWMEs(), "<result>", "<of>", "<rule1>") ||
        !containsTriple(reasoner.getCurrentState().getWMEs(), "<result>", "<of>", "<rule2a>") ||
        !containsTriple(reasoner.getCurrentState().getWMEs(), "<result>", "<of>", "<rule2b>"))
    {
        return 1;
    }


    // ---- remove rule 1. This should remove <result> <of> <rule1>
    auto newEnd = std::remove_if(rules.begin(), rules.end(),
            [](rete::ParsedRule::Ptr& r) -> bool
            {
                std::cout << "rule name: " << r->name() << " == rule1 ? " << (r->name() == "rule1") << std::endl;
                return r->name() == "rule1";
            }
    );
    rules.erase(newEnd, rules.end());

    // check if the result was retracted.
    reasoner.performInference(); // woops, inference first. Agenda, remember?

    save(reasoner.net(), "RemovingRules_2.dot");

    if (containsTriple(reasoner.getCurrentState().getWMEs(), "<result>", "<of>", "<rule1>"))
    {
        return 2;
    }

    // ---- remove rule 2a. This should remove <result> <of> <rule2a> and of <rule2b>.
    newEnd = std::remove_if(rules.begin(), rules.end(),
            [](rete::ParsedRule::Ptr& r) -> bool
            {
                return r->name() == "rule2a";
            }
    );
    rules.erase(newEnd, rules.end());

    reasoner.performInference();
    save(reasoner.net(), "RemovingRules_3.dot");

    if (containsTriple(reasoner.getCurrentState().getWMEs(), "<result>", "<of>", "<rule2a>"))
    {
        return 3;
    }

    if (containsTriple(reasoner.getCurrentState().getWMEs(), "<result>", "<of>", "<rule2b>"))
    {
        return 4;
    }


    return 0;
}
