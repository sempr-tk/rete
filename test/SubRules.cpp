#include <iostream>
#include <fstream>

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-rdf/ReteRDF.hpp"

using namespace rete;

namespace {

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


//    save(reasoner.net(), __func__ + std::string(".dot"));


bool parse_single_sub_rule()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[parent:\n\
            (?a rdfs:subClassOf ?b)\n\
            [child:\n\
                (?b rdfs:subClassOf ?c)\n\
                ->\n\
                (?a rdfs:subClassOf ?c)\n\
            ]\n\
         ]",
        reasoner.net()
    );

    std::ofstream(__func__ + std::string(".dot")) << reasoner.net().toDot();

    return true;
}

bool parse_single_sub_plus_concrete_rule()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[parent:\n\
            (?a rdfs:subClassOf ?b)\n\
            [child:\n\
                (?b rdfs:subClassOf ?c)\n\
                ->\n\
                (?a rdfs:subClassOf ?c)\n\
            ]\n\
            ->\n\
            print(?a \"is a subclass of\" ?b)\n\
         ]",
        reasoner.net()
    );

    std::ofstream(__func__ + std::string(".dot")) << reasoner.net().toDot();
    return true;
}


bool parse_multi_sub_rule()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[parent:\n\
            (?a rdfs:subClassOf ?b)\n\
            [child1:\n\
                (?b rdfs:subClassOf ?c)\n\
                ->\n\
                (?a rdfs:subClassOf ?c)\n\
            ]\n\
            [child2:\n\
                (?x rdf:type ?a)\n\
                ->\n\
                (?x rdf:type ?b)\n\
            ]\n\
         ]",
        reasoner.net()
    );

    std::ofstream(__func__ + std::string(".dot")) << reasoner.net().toDot();
    return true;
}

bool parse_multi_depth_sub_rule()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[grandparent:\n\
            (?a <foo> ?b)\n\
            [parent:\n\
                (?b <bar> ?c)\n\
                [child:\n\
                    (?c <baz> ?d) -> (?a <foobarbaz> ?d)\n\
                ]\n\
            ]\n\
        ]",
        reasoner.net()
    );

    std::ofstream(__func__ + std::string(".dot")) << reasoner.net().toDot();
    return true;
}

bool parsing_single_rule_with_else_should_fail()
{
    RuleParser p;
    Reasoner reasoner;
    try {
        auto rules = p.parseRules(
            "[(?a <foo> ?b) -> (?a <bar> ?b) else (?a <baz> ?b)]",
            reasoner.net()
        );
    } catch (RuleConstructionException& e) {
        std::cout << e.what() << std::endl;
        return true;
    }

    std::ofstream(__func__ + std::string(".dot")) << reasoner.net().toDot();
    return false;
}

bool parsing_subrule_with_else()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[foo: (?a <foo> ?b)\n"
        " [bar: (?a <bar> ?b) -> (?a <foobar> ?b) else (?a <foobaz> ?b)]\n"
        "]",
        reasoner.net()
    );

    std::ofstream(__func__ + std::string(".dot")) << reasoner.net().toDot();
    return true;
}

bool subrule_with_else_output_correct()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[foo: (?a <foo> ?b)\n"
        " [bar: (?a <bar> ?b) -> (?a <foobar> ?b) else (?a <foobaz> ?b)]\n"
        "]\n"
        ,
        reasoner.net()
    );
    std::ofstream(__func__ + std::string("0.dot")) << reasoner.net().toDot();

    auto dataRules = p.parseRules(
        "[data: true() -> (<A> <foo> <B>)]",
        reasoner.net()
    );

    reasoner.performInference();
    std::ofstream(__func__ + std::string("1.dot")) << reasoner.net().toDot();

    if (!containsTriple(reasoner, "<A>", "<foobaz>", "<B>") ||
         containsTriple(reasoner, "<A>", "<foobar>", "<B>"))
        return false;

    auto dataRules2 = p.parseRules(
        "[data2: true() -> (<A> <bar> <B>)]",
        reasoner.net()
    );

    reasoner.performInference();
    std::ofstream(__func__ + std::string("2.dot")) << reasoner.net().toDot();

    if (containsTriple(reasoner, "<A>", "<foobaz>", "<B>") ||
        !containsTriple(reasoner, "<A>", "<foobar>", "<B>"))
        return false;

    return true;
}


/*
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "",
        reasoner.net()
    );
}
*/
#define TEST(function) \
    { \
        bool ok; \
        try { \
            ok = (function)(); \
        } catch (std::exception& e) { \
            std::cout << e.what() << std::endl; \
            ok = false; \
        }\
        std::cout << (ok ? "passed test: " : "failed test: ") \
                  << #function << std::endl; \
        if (!ok) failed++; \
    }

}

int main()
{
    int failed = 0;
    TEST(parse_single_sub_rule);
    TEST(parse_single_sub_plus_concrete_rule);
    TEST(parse_multi_sub_rule);
    TEST(parse_multi_depth_sub_rule);
    TEST(parsing_single_rule_with_else_should_fail);
    TEST(parsing_subrule_with_else);
    TEST(subrule_with_else_output_correct);
    return failed;
}
