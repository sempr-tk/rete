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
    return failed;
}
