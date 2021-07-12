#include <iostream>
#include <fstream>

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-reasoner/AssertedEvidence.hpp"
#include "../rete-rdf/Triple.hpp"

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
                (<zip> <zap> <zup>)
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
                (<zip> <zap> <zup>)
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

    return 0;
}
