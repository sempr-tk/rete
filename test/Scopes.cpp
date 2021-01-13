#include <iostream>
#include <fstream>

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-rdf/ReteRDF.hpp"

using namespace rete;

namespace {

std::string to_zero_lead(const int value, const unsigned precision)
{
     std::ostringstream oss;
     oss << std::setw(precision) << std::setfill('0') << value;
     return oss.str();
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


bool no_nested_scopes()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "@PREFIX foo: <bar#>"
        "$foo : foo:bar\n"
        "[true() -> (<a> <b> $foo)]",
        reasoner.net()
    );
    reasoner.performInference();

    std::ofstream(std::string("scopes__") + to_zero_lead(__LINE__, 4) + "_" + __func__ + ".dot") << reasoner.net().toDot();

    return containsTriple(reasoner, "<a>", "<b>", "<bar#bar>");
}


bool nested_with_only_global_defs()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "@PREFIX foo: <bar#>\n"
        "$foo : foo:bar\n"
        "{\n"
        "  [true() -> (<a> <b> $foo)]\n"
        "}",
        reasoner.net()
    );
    reasoner.performInference();

    std::ofstream(std::string("scopes__") + to_zero_lead(__LINE__, 4) + "_" + __func__ + ".dot") << reasoner.net().toDot();
    return containsTriple(reasoner, "<a>", "<b>", "<bar#bar>");
}

bool one_nested_with_only_local_defs()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "{\n"
        "  @PREFIX foo: <bar#>\n"
        "  $foo : foo:bar\n"
        "  [true() -> (<a> <b> $foo)]\n"
        "}",
        reasoner.net()
    );
    reasoner.performInference();

    std::ofstream(std::string("scopes__") + to_zero_lead(__LINE__, 4) + "_" + __func__ + ".dot") << reasoner.net().toDot();
    return containsTriple(reasoner, "<a>", "<b>", "<bar#bar>");
}

bool two_parallel_nested_with_only_local_defs()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "{\n"
        "  @PREFIX foo: <bar#>\n"
        "  $foo : foo:bar\n"
        "  [true() -> (<a> <b> $foo)]\n"
        "}\n"
        "{\n"
        "  @PREFIX foo: <baz#>\n"
        "  $foo : foo:baz\n"
        "  [true() -> (<c> <d> $foo)]\n"
        "}",
        reasoner.net()
    );
    reasoner.performInference();

    std::ofstream(std::string("scopes__") + to_zero_lead(__LINE__, 4) + "_" + __func__ + ".dot") << reasoner.net().toDot();
    return
        containsTriple(reasoner, "<a>", "<b>", "<bar#bar>") &&
        containsTriple(reasoner, "<c>", "<d>", "<baz#baz>");
}

bool one_nested_using_global_and_local_defs()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "@PREFIX foo: <bar#>\n"
        "{\n"
        "  $foo : foo:bar\n"
        "  [true() -> (<a> <b> $foo)]\n"
        "}\n",
        reasoner.net()
    );
    reasoner.performInference();

    std::ofstream(std::string("scopes__") + to_zero_lead(__LINE__, 4) + "_" + __func__ + ".dot") << reasoner.net().toDot();
    return
        containsTriple(reasoner, "<a>", "<b>", "<bar#bar>");
}

bool two_parallel_nested_using_global_and_local_defs()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "@PREFIX foo: <bar#>\n"
        "{\n"
        "  $foo : foo:bar\n"
        "  [true() -> (<a> <b> $foo)]\n"
        "}\n"
        "{\n"
        "  $foo : foo:baz\n"
        "  [true() -> (<c> <d> $foo)]\n"
        "}",
        reasoner.net()
    );
    reasoner.performInference();

    std::ofstream(std::string("scopes__") + to_zero_lead(__LINE__, 4) + "_" + __func__ + ".dot") << reasoner.net().toDot();
    return
        containsTriple(reasoner, "<a>", "<b>", "<bar#bar>") &&
        containsTriple(reasoner, "<c>", "<d>", "<bar#baz>");
}

bool two_depth_nested_using_global_and_local_defs()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "@PREFIX foo: <bar#>\n"
        "{\n"
        "  @PREFIX what: <ever#>\n"
        "  $foo : foo:bar\n"
        "  [true() -> (<a> <b> $foo)]\n"
        "  {\n"
        "    $bar : foo:barest\n"
        "    $baz : what:ever\n"
        "    [true() -> ($foo $bar $baz)]\n"
        "  }\n"
        "}",
        reasoner.net()
    );
    reasoner.performInference();

    std::ofstream(std::string("scopes__") + to_zero_lead(__LINE__, 4) + "_" + __func__ + ".dot") << reasoner.net().toDot();
    return
        containsTriple(reasoner, "<a>", "<b>", "<bar#bar>") &&
        containsTriple(reasoner, "<bar#bar>", "<bar#barest>", "<ever#ever>");
}

bool one_nested_overriding_a_global_def()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "@PREFIX foo: <foo#>\n"
        "{\n"
        "  override @PREFIX foo: <bar#>\n"
        "  $foo : foo:bar\n"
        "  [true() -> (<a> <b> $foo)]\n"
        "}",
        reasoner.net()
    );
    reasoner.performInference();

    std::ofstream(std::string("scopes__") + to_zero_lead(__LINE__, 4) + "_" + __func__ + ".dot") << reasoner.net().toDot();
    return
        containsTriple(reasoner, "<a>", "<b>", "<bar#bar>");
}

bool two_depth_nested_both_overriding()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "$value : 1\n"
        "{\n"
        "  override $value : 2\n"
        "  [true() -> (<a> <b> $value)]\n"
        "  {\n"
        "    override $value : 3\n"
        "    [true() -> (<c> <d> $value)]\n"
        "  }\n"
        "}",
        reasoner.net()
    );
    reasoner.performInference();

    std::ofstream(std::string("scopes__") + to_zero_lead(__LINE__, 4) + "_" + __func__ + ".dot") << reasoner.net().toDot();
    return
        containsTriple(reasoner, "<a>", "<b>", "2") &&
        containsTriple(reasoner, "<c>", "<d>", "3");
}

bool one_nested_missing_overriding_flag_for_global_prefix_def()
{
    RuleParser p;
    Reasoner reasoner;

    try {
        auto rules = p.parseRules(
            "@PREFIX foo: <foo#>\n"
            "$b : <beee>\n"
            "{\n"
            "  @PREFIX foo: <bar#>\n"
            "  $foo : foo:bar\n"
            "  override $b : <b>\n"
            "  [true() -> (<a> $b $foo)]\n"
            "}",
            reasoner.net()
        );
    } catch (ParserException& ex) {
        return true;
    }

    return false;
}

bool one_nested_missing_overriding_flag_for_global_const_def()
{
    RuleParser p;
    Reasoner reasoner;

    try {
        auto rules = p.parseRules(
            "@PREFIX foo: <foo#>\n"
            "$b : <beee>\n"
            "{\n"
            "  override @PREFIX foo: <bar#>\n"
            "  $foo : foo:bar\n"
            "  $b : <b>\n"
            "  [true() -> (<a> $b $foo)]\n"
            "}",
            reasoner.net()
        );
    } catch (ParserException& ex) {
        return true;
    }

    return false;
}

bool wild_mix_of_plain_and_scoped_rules_without_defs()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[true() -> (<a> <b> 0)]\n"
        "{\n"
        "  [true() -> (<a> <b> 1)]\n"
        "  {\n"
        "    [true() -> (<a> <b> 2)]\n"
        "  }\n"
        "  [true() -> (<a> <b> 3)]\n"
        "  {\n"
        "    {\n"
        "      [true() -> (<a> <b> 4)]\n"
        "    }\n"
        "    {\n"
        "      [true() -> (<a> <b> 5)]\n"
        "    }\n"
        "  }\n"
        "  [true() -> (<a> <b> 6)]\n"
        "  {\n"
        "    [true() -> (<a> <b> 7)]\n"
        "    [true() -> (<a> <b> 8)]\n"
        "  }\n"
        "  {\n"
        "    {\n"
        "      {\n"
        "        {\n"
        "          [true() -> (<a> <b> 9)]\n"
        "        }\n"
        "      }\n"
        "    }\n"
        "  }\n"
        "}",
        reasoner.net()
    );
    reasoner.performInference();

    std::ofstream(std::string("scopes__") + to_zero_lead(__LINE__, 4) + "_" + __func__ + ".dot") << reasoner.net().toDot();
    return
        containsTriple(reasoner, "<a>", "<b>", "1") &&
        containsTriple(reasoner, "<a>", "<b>", "2") &&
        containsTriple(reasoner, "<a>", "<b>", "3") &&
        containsTriple(reasoner, "<a>", "<b>", "4") &&
        containsTriple(reasoner, "<a>", "<b>", "5") &&
        containsTriple(reasoner, "<a>", "<b>", "6") &&
        containsTriple(reasoner, "<a>", "<b>", "7") &&
        containsTriple(reasoner, "<a>", "<b>", "8") &&
        containsTriple(reasoner, "<a>", "<b>", "9");
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
    TEST(no_nested_scopes);
    TEST(nested_with_only_global_defs);
    TEST(one_nested_with_only_local_defs);
    TEST(two_parallel_nested_with_only_local_defs);
    TEST(one_nested_using_global_and_local_defs);
    TEST(two_parallel_nested_using_global_and_local_defs);
    TEST(two_depth_nested_using_global_and_local_defs);
    TEST(one_nested_overriding_a_global_def);
    TEST(two_depth_nested_both_overriding);
    TEST(one_nested_missing_overriding_flag_for_global_prefix_def);
    TEST(one_nested_missing_overriding_flag_for_global_const_def);
    TEST(wild_mix_of_plain_and_scoped_rules_without_defs);

    return failed;
}
