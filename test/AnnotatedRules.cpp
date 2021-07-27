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
    }


    {
        auto rules = p.parseRules(
            R"foo(
                [subclass:
                    {
                        (?a <subClassOf> ?b)
                        /* {a} is a subclass of {b} */
                    }
                    [transitive:
                        {
                            (?b <subClassOf> ?c)
                            /* {b} is a subclass of {c} */
                        }
                        ->
                        {
                            (?a <subClassOf> ?c)
                            /* The subClassOf property is transitive, hence
                               {a} being a subclass of {b} being a subclass of {c}
                               also means that {a} is a subclass of {c} */
                        }
                    ]
                    [instance:
                        {
                            (?x <type> ?a)
                            /* {x} is of type {a} */
                        }
                        ->
                        {
                            (?x <type> ?b)
                            /* Because {a} is a subclass of {b}, {x} is also of
                               type {b} */
                        }
                    ]
                ]

                [data:
                    true()
                    ->
                    {
                        (<A> <subClassOf> <B>),
                        (<B> <subClassOf> <C>),
                        (<x> <type> <A>)
                        /* Simple facts. */
                    }
                ]
            )foo"
            ,
            reasoner.net()
        );

        reasoner.performInference();
        auto triple = std::make_shared<rete::Triple>("<x>", "<type>", "<C>");

        ExplanationToJSONVisitor visitor;
        reasoner.getCurrentState().traverseExplanation(triple, visitor);

        std::ofstream("annotated_explain_1.json") << visitor.json().dump(4);
    }

    {
        auto rules = p.parseRules(
            R"foo(
                [
                    {
                        (<global> <today> ?date)
                        /* Today is {date}. */
                    },
                    {
                        (?weatherData <type> <WeatherData>),
                        (?weatherData <date> ?date),
                        (?weatherData <weather> ?weather)
                        /* The weather on {date} is {weather}. */
                    },
                    {
                        (?activityData <type> <Activity>),
                        (?activityData <applicableInWeather> ?weather),
                        (?activityData <name> ?activity)
                        /* {activity} is possible when it is {weather}. */
                    }
                    ->
                    {
                        (<global> <recommend> ?activity)
                        /* Since it is {weather} today, we recommend {activity}. */
                    }
                ]
            )foo",
            reasoner.net()
        );

        auto data = p.parseRules(
            R"foo(
                [data:
                    true()
                    ->
                    {
                        (<global> <today> "27.07.2021")
                        /* Just the current date */
                    },
                    {
                        (<w1> <type> <WeatherData>), (<w1> <date> "26.07.2021"), (<w1> <weather> "sunny"),
                        (<w2> <type> <WeatherData>), (<w2> <date> "27.07.2021"), (<w2> <weather> "rainy")
                        /* Weather forecast for a few days */
                    },
                    {
                        (<a1> <type> <Activity>), (<a1> <applicableInWeather> "sunny"), (<a1> <name> "cycling"),
                        (<a2> <type> <Activity>), (<a2> <applicableInWeather> "rainy"), (<a2> <name> "reading a book")
                        /* Basic activity information */
                    }
                ]
            )foo",
            reasoner.net()
        );

        reasoner.performInference();
        auto triple = std::make_shared<rete::Triple>("<global>", "<recommend>", "\"reading a book\"");

        ExplanationToJSONVisitor visitor;
        reasoner.getCurrentState().traverseExplanation(triple, visitor);

        std::ofstream("annotated_explain_2.json") << visitor.json().dump(4);
    }


    return 0;
}
