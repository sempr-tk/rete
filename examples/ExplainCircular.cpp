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

    p.parseRules(
        "[init: (<A> <B> <C>) -> (<a> <foo> <b>), (<a> <bar> <b>)]"
        "[circle1: (<a> <foo> <b>) -> (<a> <bar> <b>)]"
        "[circle2: (<a> <bar> <b>) -> (<a> <foo> <b>)]"
        "[end: (<a> <foo> <b>), (<a> <bar> <b>) -> (<X> <Y> <Z>)]"
        ,
        reasoner.net()
    );
    // p.parseRules(
    //     "[rule1: (<A> <B> <C>) -> (<A> <equivalent> <B>)]"
    //     "[rule2: (?a <equivalent> ?b) -> (?b <equivalent> ?a)]"
    //     "[rule3: (<B> <equivalent> <A>) -> (<X> <Y> <Z>)]"
    //     "[rule4: (<X> <Y> <Z>) -> (<B> <equivalent> <A>)]",
    //     reasoner.net()
    // );

    auto source = std::make_shared<AssertedEvidence>("fact-group-1");
    auto t1 = std::make_shared<Triple>("<A>", "<B>", "<C>");
    reasoner.addEvidence(t1, source);

    reasoner.performInference();

    save(reasoner.net(), "ExplainNetworkCircular.dot");

    // explain why (<B> <equivalent> <A>)
    ExplanationToDotVisitor visitor;
    auto state = reasoner.getCurrentState();
    // auto t = std::make_shared<Triple>("<B>", "<equivalent>", "<A>");
    auto t = std::make_shared<Triple>("<X>", "<Y>", "<Z>");
    state.traverseExplanation(t, visitor);

    std::ofstream expl("ExplanationCircular.dot");
    expl << visitor.str();
    expl.close();


    return 0;
}
