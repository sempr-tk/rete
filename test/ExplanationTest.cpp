#include <iostream>
#include <fstream>

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-reasoner/AssertedEvidence.hpp"
#include "../rete-rdf/Triple.hpp"
#include "../rete-core/Accessors.hpp"
#include "../rete-reasoner/NodeBuilder.hpp"
#include "../rete-reasoner/Exceptions.hpp"
#include "../rete-reasoner/ExplanationToDotVisitor.hpp"
#include "../rete-reasoner/ExplanationToJSONVisitor.hpp"

using namespace rete;


int main()
{
    RuleParser p;
    Reasoner reasoner;


    /**
    */
    auto rules = p.parseRules(
        "[ontology: true() -> (<A> <subClassOf> <B>), (<B> <subClassOf> <C>)]"
        "[data: true() -> (<a> <type> <A>), (<b> <type> <B>)]"
        "[subclass:"
        "   (?a <subClassOf> ?b)"
        "   [transitive:"
        "       (?b <subClassOf> ?c) -> (?a <subClassOf> ?c)]"
        "   [type:"
        "       (?x <type> ?a) -> (?x <type> ?b)]"
        "]"
        ,
        reasoner.net()
    );

    reasoner.performInference();

    auto triple = std::make_shared<rete::Triple>("<a>", "<type>", "<C>");

    ExplanationToDotVisitor visitor;
    reasoner.getCurrentState().traverseExplanation(triple, visitor);
    std::ofstream("explain_test.dot") << visitor.str();

    for (auto& wme : reasoner.getCurrentState().getWMEs())
    {
        std::cout << wme->toString() << std::endl;
    }

    ExplanationToJSONVisitor jsonVisitor;
    reasoner.getCurrentState().traverseExplanation(triple, jsonVisitor);
    std::ofstream("explain_test.json") << jsonVisitor.json().dump(4);

    return 0;
}
