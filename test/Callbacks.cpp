#include <iostream>
#include <fstream>

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-rdf/ReteRDF.hpp"

#include "../rete-reasoner/CallbackEffectBuilder.hpp"
#include <cmath>

using namespace rete;

namespace {


std::string to_zero_lead(const int value, const unsigned precision)
{
     std::ostringstream oss;
     oss << std::setw(precision) << std::setfill('0') << value;
     return oss.str();
}

bool callbackNoArgs_lambda()
{
    RuleParser p;
    Reasoner r;

    auto cb = [](PropagationFlag flag) -> void
    {
        std::cout << "no args lambda - prop flag: " << flag << std::endl;
    };

    p.registerNodeBuilder(makeCallbackBuilder("myCallback", cb));

    auto rules = p.parseRules(
        "[true() -> myCallback()]",
        r.net()
    );

    r.performInference();

    std::ofstream(std::string("cb_") + to_zero_lead(__LINE__, 4) + "_" + __func__ + ".dot") << r.net().toDot();

    return true;
}


bool callbackOneStrArg_lambda()
{
    RuleParser p;
    Reasoner r;

    std::string result;

    auto cb = [&result](PropagationFlag /*flag*/, std::string str) -> void
    {
        std::cout << "callback lambda one str arg: " << str << std::endl;
        result = str;
    };

    p.registerNodeBuilder(makeCallbackBuilder("callback", cb));

    auto rules = p.parseRules(
        "[true() -> callback(\"Hello, World!\")]",
        r.net()
    );

    std::ofstream(std::string("cb_") + to_zero_lead(__LINE__, 4) + "_" + __func__ + ".dot") << r.net().toDot();

    r.performInference();

    return result == "Hello, World!";
}


void one_str_arg_free_func(PropagationFlag /*flag*/, std::string str)
{
    std::cout << "callback free func one str arg: " << str << std::endl; 
}

bool callbackOneStrArg_free_func()
{
    RuleParser p;
    Reasoner r;

    p.registerNodeBuilder(makeCallbackBuilder("callback", one_str_arg_free_func));

    auto rules = p.parseRules(
        "[true() -> callback(\"Hello, World!\")]",
        r.net()
    );

    std::ofstream(std::string("cb_") + to_zero_lead(__LINE__, 4) + "_" + __func__ + ".dot") << r.net().toDot();

    r.performInference();

    return true;
}

bool callback_str_int_float_args_lambda()
{
    RuleParser p;
    Reasoner r;

    std::string rStr;
    int rInt;
    float rFloat;

    auto cb = 
    [&rStr, &rInt, &rFloat]
    (PropagationFlag /*flag*/, std::string str, int i, float f) -> void
    {
        std::cout << "callback lambda str int float arg: "
                  << str << ", " << i << ", " << f
                  << std::endl;
        rStr = str;
        rInt = i;
        rFloat = f;
    };

    p.registerNodeBuilder(makeCallbackBuilder("callback", cb));

    auto rules = p.parseRules(
        "[true() -> callback(\"Hello, World!\" 42 3.14)]",
        r.net()
    );

    std::ofstream(std::string("cb_") + to_zero_lead(__LINE__, 4) + "_" + __func__ + ".dot") << r.net().toDot();

    r.performInference();

    return rStr == "Hello, World!" && rInt == 42 && fabs(rFloat-3.14) < 1e-5;
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
    TEST(callbackNoArgs_lambda);
    TEST(callbackOneStrArg_lambda);
    TEST(callbackOneStrArg_free_func);
    TEST(callback_str_int_float_args_lambda);
    return 0;
}