#include <iostream>
#include <fstream>

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-reasoner/AssertedEvidence.hpp"
#include "../rete-rdf/Triple.hpp"
#include "../rete-reasoner/ExplanationToJSONVisitor.hpp"

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
     * Just a simple check if the parser allows to annotate groups of conditions.
     * No check on output for now.
    */
    auto rules = p.parseRules(
        R"foo(
            [single_group:
                {
                    (?x <bar> ?bar),
                    (?x <baz> ?baz),
                    ge(?bar ?baz)
                    /* {x} is a foo with its <bar> ({bar}) greater or equal its <baz> ({baz}). */
                }
                ->
                {
                    (<zip> <zap> <zup>)
                    /* Because {x} has a bar-value of {bar} which is less than
                       its baz-value of {baz}, we follow zip-zap-zup! Obvious, right? */
                }
            ]

            [with_subgroup:
                {
                    (?a <subClassOf> ?b)
                    /* {a} is a subclass of {b} */
                }
                [sub:
                    { (?x <type> ?a) /* {x} is of type {a} */ }
                    ->
                    {
                        (?x <type> ?b)
                        /* Because {x} is of type {a}, which is a subclass of {b}, {x} is also of type {b} */
                    }
                ]
            ]

            [two_groups:
                {
                    (?x <bar> 5)
                    /* <bar> of {x} is 5 */
                }
                {
                    (?x <baz> ?baz), lt(?baz 5)
                    /* <baz> of {x} is < 5 */
                }
                ->
                {
                    (<zip> <zap> <zup>)
                    /* Because there is an instance ({x}) with a bar-value of exactly 5
                       which also has a baz-value which is less than 5, namely {baz}, we
                       can infer zip-zap-zup. */
                }
            ]

            [two_groups_and_some_unannotated:
                {
                    (?x <bar> 5)
                    /* <bar> of {x} is 5 */
                }
                (?x <type> <foo>),
                (<foo> <subClassOf> ?something),
                {
                    (?x <baz> ?baz), lt(?baz 5)
                    /* <baz> of {x} is < 5 */
                }
                ->
                (<zip> <zap> <zup>)
            ]
        )foo",
        reasoner.net()
    );

    {
        // test data
        auto datarules = p.parseRules(
            "[data: true() -> (<Entity_0> <bar> 5), (<Entity_0> <baz> 2)]",
            reasoner.net()
        );

        reasoner.performInference();

        auto triple = std::make_shared<rete::Triple>("<zip>", "<zap>", "<zup>");

        ExplanationToJSONVisitor visitor;
        reasoner.getCurrentState().traverseExplanation(triple, visitor);

        std::ofstream("annotated_explain_0.json") << visitor.json().dump(4);
    }

    return 0;
}
