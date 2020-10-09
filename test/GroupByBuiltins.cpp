#include <iostream>
#include <fstream>

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-rdf/ReteRDF.hpp"

using namespace rete;


void save(Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}

bool containsTriple(Reasoner& reasoner,
                    const std::string& s, const std::string& p, const std::string& o)
{
    auto wmes = reasoner.getCurrentState().getWMEs();
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



bool sum_bulk()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[(?player <scored> ?points),"
        " GROUP BY (?player),"
        " SumBulk(?total ?points)"
        " -> (?player <total> ?total)]",
        reasoner.net()
    );

    auto addScore = [&](const std::string& player, int score)
    {
        auto triple = std::make_shared<Triple>(
                            "<" + player + ">",
                            "<scored>",
                            std::to_string(score));
        auto ev = std::make_shared<AssertedEvidence>("asserted");

        reasoner.addEvidence(triple, ev);
    };

    addScore("p1", 10);
    addScore("p1", 5);
    addScore("p1", 7);
    addScore("p2", 100);
    addScore("p2", -23);

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<p1>", "<total>", std::to_string(22.f)) &&
           containsTriple(reasoner, "<p2>", "<total>", std::to_string(77.f));
}


bool sum_bulk_retract()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[(?player <scored> ?points),"
        " GROUP BY (?player),"
        " SumBulk(?total ?points)"
        " -> (?player <total> ?total)]",
        reasoner.net()
    );

    auto ev = std::make_shared<AssertedEvidence>("asserted");
    auto addScore = [&](const std::string& player, int score) -> Triple::Ptr
    {
        auto triple = std::make_shared<Triple>(
                            "<" + player + ">",
                            "<scored>",
                            std::to_string(score));

        reasoner.addEvidence(triple, ev);

        return triple;
    };

    auto t10 = addScore("p1", 10);
    auto t5 = addScore("p1", 5);
    auto t7 = addScore("p1", 7);

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string("_0.dot"));

    // see sum_bulk()
    assert(containsTriple(reasoner, "<p1>", "<total>", std::to_string(22.f)));

    reasoner.removeEvidence(t5, ev);
    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string("_1.dot"));

    if (!containsTriple(reasoner, "<p1>", "<total>", std::to_string(17.f)) ||
         reasoner.getCurrentState().numWMEs() != 3)
        return false;
    else
    {
        reasoner.removeEvidence(t7, ev);
        reasoner.performInference();
        save(reasoner.net(), __func__ + std::string("_2.dot"));

        if (!containsTriple(reasoner, "<p1>", "<total>", std::to_string(10.f)) ||
             reasoner.getCurrentState().numWMEs() != 2)
            return false;
        else
        {
            reasoner.removeEvidence(t10, ev);
            reasoner.performInference();
            save(reasoner.net(), __func__ + std::string("_3.dot"));

            return reasoner.getCurrentState().numWMEs() == 0;
        }
    }
}


bool mul_bulk()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[(?situation <has> ?event), (?event <probability> ?ep),"
        " GROUP BY (?situation), MulBulk(?sp ?ep)"
        " -> (?situation <probability> ?sp)]",
        reasoner.net()
    );

    auto events = p.parseRules(
        "[true() ->"
        " (<e1> <probability> 0.5),"
        " (<e2> <probability> 0.5),"
        " (<e3> <probability> 0.1),"
        " (<e4> <probability> 0.2)]",
        reasoner.net()
    );

    auto situations = p.parseRules(
        "[true() ->"
        " (<s1> <has> <e1>), (<s1> <has> <e2>),"
        " (<s2> <has> <e3>), (<s2> <has> <e4>),"
        " (<s3> <has> <e1>), (<s3> <has> <e2>), (<s3> <has> <e3>)]",
        reasoner.net()
    );

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<s1>", "<probability>", std::to_string(0.25f)) &&
           containsTriple(reasoner, "<s2>", "<probability>", std::to_string(0.02f)) &&
           containsTriple(reasoner, "<s3>", "<probability>", std::to_string(0.025f));

}


bool count()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[(?player <inTeam> ?team),"
        " GROUP BY (?team),"
        " count(?count ?player)"
        " -> (?team <playerCount> ?count)]",
        reasoner.net()
    );


    // add another unimportant rule to see if group-by nodes are reused
    auto rules2 = p.parseRules(
        "[(?player <inTeam> ?team),"
        " GROUP BY (?team),"
        " count(?count ?player)"
        " -> print(?team \"<playerCount>\" ?count)]",
        reasoner.net()
    );


    auto data = p.parseRules(
        "[true() ->"
        " (<pb1> <inTeam> <blue>),"
        " (<pb2> <inTeam> <blue>),"
        " (<pb3> <inTeam> <blue>),"
        " (<pr1> <inTeam> <red>),"
        " (<pr2> <inTeam> <red>),"
        " (<pr3> <inTeam> <red>),"
        " (<pr4> <inTeam> <red>)]",
        reasoner.net()
    );

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<blue>", "<playerCount>", std::to_string(3)) &&
           containsTriple(reasoner, "<red>", "<playerCount>", std::to_string(4));
}


bool count_and_compare()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[(?player <inTeam> ?team),"
        " GROUP BY (?team),"
        " count(?count ?player),"
        " ge(?count 4)"
        " -> (?team <type> <BigTeam>)]"
        ""
        "[(?player <inTeam> ?team),"
        " GROUP BY(?team),"
        " count(?count ?player),"
        " lt(?count 4)"
        " -> (?team <type> <SmallTeam>)]",
        reasoner.net()
    );

    auto data = p.parseRules(
        "[true() ->"
        " (<pb1> <inTeam> <blue>),"
        " (<pb2> <inTeam> <blue>),"
        " (<pb3> <inTeam> <blue>),"
        " (<pr1> <inTeam> <red>),"
        " (<pr2> <inTeam> <red>),"
        " (<pr3> <inTeam> <red>),"
        " (<pr4> <inTeam> <red>)]",
        reasoner.net()
    );

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<blue>", "<type>", "<SmallTeam>") &&
           containsTriple(reasoner, "<red>", "<type>", "<BigTeam>");

}

#define TEST(function) \
    { \
        bool ok = (function)(); \
        std::cout << (ok ? "passed test: " : "failed test: ") \
                  << #function << std::endl; \
        if (!ok) failed++; \
    }



int main()
{
    int failed = 0;
    TEST(sum_bulk);
    TEST(mul_bulk);
    TEST(sum_bulk_retract);
    TEST(count);
    TEST(count_and_compare);
    return failed;
}
