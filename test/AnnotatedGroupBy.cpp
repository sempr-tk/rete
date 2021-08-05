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

    return 0;
}
