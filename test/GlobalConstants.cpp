#include <iostream>
#include <fstream>

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-rdf/ReteRDF.hpp"

using namespace rete;

namespace {

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


//    save(reasoner.net(), __func__ + std::string(".dot"));


bool global_const_allows_string()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "$foo : \"Hello, World\"\n"
        "[true() -> print(\"dummy\")]",
        reasoner.net()
    );

    return true;
}


bool global_const_allows_number()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "$foo : 3.14159"
        "[true() -> print(\"dummy\")]",
        reasoner.net()
    );

    return true;
}

bool global_const_allows_full_uri()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "$foo : <http://something.com/plex/this#that>"
        "[true() -> print(\"dummy\")]",
        reasoner.net()
    );

    return true;
}

bool global_const_allows_shorthand_uri()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "@PREFIX something: <http://something.com/plex/this#>\n"
        "$foo : something:that"
        "[true() -> print(\"dummy\")]",
        reasoner.net()
    );

    return true;
}

bool global_const_string_can_be_used_in_triple_condition()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "$foo : \"Hello, World!\""
        "[(?a ?b $foo) -> (<test> <is> <successfull>)]",
        reasoner.net()
    );

    auto data = p.parseRules(
        "[true() -> (<foo> <bar> \"Hello, World!\")]",
        reasoner.net()
    );

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<test>", "<is>", "<successfull>");
}


bool global_const_number_can_be_used_in_triple_condition()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "$foo : 3.14159"
        "[(?a ?b $foo) -> (<test> <is> <successfull>)]",
        reasoner.net()
    );

    auto data = p.parseRules(
        "[true() -> (<foo> <bar> 3.14159)]",
        reasoner.net()
    );

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<test>", "<is>", "<successfull>");
}

bool global_const_shorthand_uri_can_be_used_in_triple_condition()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "@PREFIX something: <http://something.com/plex/this#>\n"
        "$foo : something:that\n"
        "[(?a ?b $foo) -> (<test> <is> <successfull>)]",
        reasoner.net()
    );

    auto data = p.parseRules(
        "[true() -> (<foo> <bar> <http://something.com/plex/this#that>)]",
        reasoner.net()
    );

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<test>", "<is>", "<successfull>");
}

bool global_const_number_can_be_used_in_math_builtin()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "$foo : 21\n"
        "[true(), sum(?s $foo $foo) -> (<the-answer> <is> ?s)]",
        reasoner.net()
    );

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<the-answer>", "<is>", std::to_string(42));
}

bool global_const_string_can_be_used_in_triple_consequence()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "$foo : \"Hello, World!\"\n"
        "[true() -> (<greeting> <message> $foo)]",
        reasoner.net()
    );

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<greeting>", "<message>", "\"Hello, World!\"");
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
    TEST(global_const_allows_string);
    TEST(global_const_allows_number);
    TEST(global_const_allows_full_uri);
    TEST(global_const_allows_shorthand_uri);
    TEST(global_const_string_can_be_used_in_triple_condition);
    TEST(global_const_number_can_be_used_in_triple_condition);
    TEST(global_const_shorthand_uri_can_be_used_in_triple_condition);
    TEST(global_const_number_can_be_used_in_math_builtin);
    TEST(global_const_string_can_be_used_in_triple_consequence);

    return failed;
}
