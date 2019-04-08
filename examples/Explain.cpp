#include <iostream>
#include <fstream>

#include "../rete-rdf/ReteRDF.hpp"

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-reasoner/AssertedEvidence.hpp"
#include "../rete-reasoner/ExplanationToDotVisitor.hpp"


using namespace rete;


void save(Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}

Triple::Ptr color(const std::string& color)
{
    return Triple::Ptr(new Triple(color, "<type>", "<color>"));
}

Triple::Ptr are(const std::string& a, const std::string& b)
{
    return Triple::Ptr(new Triple(a, "<are>", b));
}

Triple::Ptr likes(const std::string& a, const std::string& b)
{
    return Triple::Ptr(new Triple(a, "<likes>", b));
}

int main()
{
    RuleParser p;
    Reasoner reasoner;

    // if one likes a color, and something is of that color, one likes that thing; and
    // if one likes someone, and that someone likes something, one likes that thing, too.
    p.parseRules(
    "[likeColoredThings: (?a <type> <color>), (?b <likes> ?a), (?c <are> ?a) -> (?b <likes> ?c)]"
    "[transitiveLikes: (?a <likes> ?b), (?b <likes> ?c) -> (?a <likes> ?c)]",
        reasoner.net()
    );

    Evidence::Ptr evidence(new AssertedEvidence("fact-group-1"));

    // assert that firetrucks are red, frogs are green,
    // alice likes red things, bob likes green things, and bob likes alice.
    reasoner.addEvidence(color("<red>"), evidence);
    reasoner.addEvidence(color("<blue>"), evidence);
    reasoner.addEvidence(are("<roses>", "<red>"), evidence);
    reasoner.addEvidence(are("<violets>", "<blue>"), evidence);
    reasoner.addEvidence(likes("<alice>", "<red>"), evidence);
    reasoner.addEvidence(likes("<bob>", "<blue>"), evidence);
    reasoner.addEvidence(likes("<bob>", "<alice>"), evidence);


    reasoner.performInference();

    save(reasoner.net(), "ExplainNetwork.dot");


    // explain why bob likes firetrucks.
    ExplanationToDotVisitor visitor;
    auto state = reasoner.getCurrentState();
    auto t = likes("<bob>", "<roses>");

    for (int i = 0; i < 5; i++)
    {
        visitor.reset();
        visitor.setMaxDepth(i);
        state.traverseExplanation(t, visitor);

        std::ofstream expl("Explanation" + std::to_string(i) + ".dot");

        using VizSettings = rete::ExplanationToDotVisitor::VizSettings;
        expl << visitor.str(VizSettings::FORCE_LOWEST_RANK);
        expl.close();
    }


    std::ofstream expl("Explanation.dot");
    expl << visitor.str(rete::ExplanationToDotVisitor::VizSettings::TOP_DOWN);
    expl.close();

    return 0;
}
