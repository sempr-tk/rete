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

    {
        /**
         * Just a simple check if the parser allows to annotate groups of conditions.
         * No check on output for now.
        */
        auto rules = p.parseRules(
            R"foo(
                [noRedCars:
                    {
                        (?c <type> <Car>),
                        (?c <color> ?color)
                        /* {c} is a {color} car */
                    }
                    {
                        noValue {
                            (?x <type> <Car>),
                            (?x <color> ?color),
                            neq(?x ?c)
                        }
                        /* There is no other {color} car, only {c} */
                    }
                    ->
                    (?c <is-the-only-car-that-is> ?color)
                ]
            )foo",
            reasoner.net()
        );

        {
            // test data
            auto datarules = p.parseRules(
                R"foo(
                    [data: true()
                        ->
                        (<c1> <type> <Car>), (<c1> <color> <blue>),
                        (<c2> <type> <Car>), (<c2> <color> <red>),
                        (<c3> <type> <Car>), (<c3> <color> <blue>)]
                )foo",
                reasoner.net()
            );

            reasoner.performInference();

            auto triple = std::make_shared<rete::Triple>("<c2>", "<is-the-only-car-that-is>", "<red>");

            ExplanationToJSONVisitor visitor;
            reasoner.getCurrentState().traverseExplanation(triple, visitor);

            std::ofstream("annotated_novalue_explain_0.json") << visitor.json().dump(4);
        }
    }



    return 0;
}
