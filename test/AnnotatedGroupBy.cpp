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
                [teamScore:
                    {
                        (?team <type> <Team>),
                        (?player <inTeam> ?team),
                        (?player <score> ?pscore)
                        /* {player} scored {pscore} for {team} */
                    },
                    {
                        GROUP BY (?team),
                        SumBulk(?tscore ?pscore)
                        /* The sum of the scores of all players in {team} is {tscore} */
                    }
                    ->
                    (?team <score> ?tscore)
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
                        (<TeamRed> <type> <Team>),
                        (<r1> <inTeam> <TeamRed>), (<r1> <score> 1),
                        (<r2> <inTeam> <TeamRed>), (<r2> <score> 2),
                        (<r3> <inTeam> <TeamRed>), (<r3> <score> 3),
                    ]
                )foo",
                reasoner.net()
            );

            reasoner.performInference();

            auto triple = std::make_shared<rete::Triple>("<TeamRed>", "<score>", std::to_string(6.0));

            ExplanationToJSONVisitor visitor;
            reasoner.getCurrentState().traverseExplanation(triple, visitor);

            std::ofstream("annotated_groupby_explain_0.json") << visitor.json().dump(4);
        }
    }


    {
        /**
         * Modified: Without annotation of grouped conditions
        */
        auto rules = p.parseRules(
            R"foo(
                [teamScoreLessAnnotated:
                    (?team <type> <Team>),
                    (?player <inTeam> ?team),
                    (?player <score> ?pscore),

                    {
                        GROUP BY (?team),
                        SumBulk(?tscore ?pscore)
                        /* The sum of the scores of all players in {team} is {tscore} */
                    }
                    ->
                    (?team <score> ?tscore)
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
                        (<TeamRed> <type> <Team>),
                        (<r1> <inTeam> <TeamRed>), (<r1> <score> 1),
                        (<r2> <inTeam> <TeamRed>), (<r2> <score> 2),
                        (<r3> <inTeam> <TeamRed>), (<r3> <score> 3),
                    ]
                )foo",
                reasoner.net()
            );

            reasoner.performInference();

            auto triple = std::make_shared<rete::Triple>("<TeamRed>", "<score>", std::to_string(6.0));

            ExplanationToJSONVisitor visitor;
            reasoner.getCurrentState().traverseExplanation(triple, visitor);

            std::ofstream("annotated_groupby_explain_1.json") << visitor.json().dump(4);
        }
    }

        {
        /**
         * Modified: Partial annotation of grouped conditions
        */
        auto rules = p.parseRules(
            R"foo(
                [teamScoreLessAnnotated:
                    {
                        (?team <type> <Team>),
                        (?player <inTeam> ?team)
                        /* {player} plays for {team} */
                    },
                    (?player <score> ?pscore),

                    {
                        GROUP BY (?team),
                        SumBulk(?tscore ?pscore)
                        /* The sum of the scores of all players in {team} is {tscore} */
                    }
                    ->
                    (?team <score> ?tscore)
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
                        (<TeamRed> <type> <Team>),
                        (<r1> <inTeam> <TeamRed>), (<r1> <score> 1),
                        (<r2> <inTeam> <TeamRed>), (<r2> <score> 2),
                        (<r3> <inTeam> <TeamRed>), (<r3> <score> 3),
                    ]
                )foo",
                reasoner.net()
            );

            reasoner.performInference();

            auto triple = std::make_shared<rete::Triple>("<TeamRed>", "<score>", std::to_string(6.0));

            ExplanationToJSONVisitor visitor;
            reasoner.getCurrentState().traverseExplanation(triple, visitor);

            std::ofstream("annotated_groupby_explain_2.json") << visitor.json().dump(4);
        }
    }


        {
        /**
         * Try explaining nested group-by
         */
        auto rules = p.parseRules(
            R"foo(
                [teamScore:
                    {
                        (?player <score> ?pscore)
                        /* {player} scored {pscore} once. */
                    },
                    {
                        GROUP BY (?player),
                        SumBulk(?playerTotal ?pscore)
                        /* {player} scored a total of {playerTotal}. */
                    },
                    {
                        (?player <inTeam> ?team),
                        (?team <type> <Team>)
                        /* {player} plays for {team}. */
                    },
                    {
                        GROUP BY (?team),
                        SumBulk(?teamScore ?playerTotal)
                        /* The sum of total scores of players in {team} is {teamScore}. */
                    }
                    ->
                    {
                        (?team <score> ?teamScore)
                        /* The teams score is computed from the sum of the
                           total scores of the players that play for that team */
                    }
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
                        (<TeamRed> <type> <Team>),
                        (<r1> <inTeam> <TeamRed>), (<r1> <score> 0.7), (<r1> <score> 0.3),
                        (<r2> <inTeam> <TeamRed>), (<r2> <score> 1.5), (<r2> <score> 0.3), (<r2> <score> 0.2),
                        (<r3> <inTeam> <TeamRed>), (<r3> <score> 2), (<r3> <score> 1)
                    ]
                )foo",
                reasoner.net()
            );

            reasoner.performInference();

            auto triple = std::make_shared<rete::Triple>("<TeamRed>", "<score>", std::to_string(6.0));

            ExplanationToJSONVisitor visitor;
            reasoner.getCurrentState().traverseExplanation(triple, visitor);

            std::ofstream("annotated_groupby_explain_3.json") << visitor.json().dump(4);
            std::ofstream("annotated_groupby_network_3.dot") << reasoner.net().toDot();
        }
    }

    return 0;
}
