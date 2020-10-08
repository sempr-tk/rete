#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-reasoner/AssertedEvidence.hpp"
#include "../rete-rdf/Triple.hpp"

#include <fstream>

using namespace rete;


void save(Network& net, const std::string& filename)
{
    std::ofstream out("makeSkolem_" + filename);
    out << net.toDot();
    out.close();
}


bool skolem_from_const_string()
{
    RuleParser p;
    Reasoner reasoner;

    auto rules = p.parseRules(
        "[true(), makeSkolem(?skolem \"hello\") -> (?skolem <skolemFrom> \"hello\")]",
        reasoner.net()
    );

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string(".dot"));

    // there should be exactly one triple inferred.
    return reasoner.getCurrentState().getWMEs().size() == 1;
}

bool skolem_from_const_number()
{
    RuleParser p;
    Reasoner reasoner;

    auto rules = p.parseRules(
        "[true(), makeSkolem(?skolem 3.1415) -> (?skolem <skolemFrom> 3.1415)]",
        reasoner.net()
    );

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string(".dot"));

    // there should be exactly one triple inferred.
    return reasoner.getCurrentState().getWMEs().size() == 1;
}

bool two_skolems_with_same_arguments_are_equal()
{
    RuleParser p;
    Reasoner reasoner;

    auto rules = p.parseRules(
        // a bit bloated on purpose, to prevent node-reusage and actually have two
        // different makeSkolem nodes with different input triples
        "[r1: (<input1> <value> ?val), makeSkolem(?skolem ?val) -> (<s1> <value> ?skolem)]"
        "[r2: (<input2> <value> ?val), makeSkolem(?skolem ?val) -> (<s2> <value> ?skolem)]"
        // check if the created skolems are equal
        "[r3: (<s1> <value> ?a), (<s2> <value> ?a) -> (<skolems> <are> <equal>)]"
        // insert the needed data
        "[data: true() -> (<input1> <value> \"hello\"), (<input2> <value> \"hello\")]",
        reasoner.net()
    );

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string(".dot"));

    // check if the equality-rule was triggered by counting WMEs:
    // - 2x (<input_> <value> "hello")
    // - 2x (<s_> <value> ?skolem)
    // - 1x (<skolems> <are> <equal>)
    return reasoner.getCurrentState().getWMEs().size() == 5;
}

/// this is a copy of two_skolems_with_same_arguments_are_equal,
/// only the data rule is modified to insert different values, and the check
/// at the end requires only 4 WMEs to be present (so that the equality-rule
/// was *not* triggered)
bool two_skolems_with_different_arguments_are_unequal()
{
    RuleParser p;
    Reasoner reasoner;

    auto rules = p.parseRules(
        // a bit bloated on purpose, to prevent node-reusage and actually have two
        // different makeSkolem nodes with different input triples
        "[r1: (<input1> <value> ?val), makeSkolem(?skolem ?val) -> (<s1> <value> ?skolem)]"
        "[r2: (<input2> <value> ?val), makeSkolem(?skolem ?val) -> (<s2> <value> ?skolem)]"
        // check if the created skolems are equal
        "[r3: (<s1> <value> ?a), (<s2> <value> ?a) -> (<skolems> <are> <equal>)]"
        // insert the needed data
        "[data: true() -> (<input1> <value> \"hello\"), (<input2> <value> \"world\")]",
        reasoner.net()
    );

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string(".dot"));

    // check if the equality-rule was *not* triggered by counting WMEs:
    // - 2x (<input_> <value> "hello" / "world")
    // - 2x (<s_> <value> ?skolem)
    // - hopefully not: 1x (<skolems> <are> <equal>)
    return reasoner.getCurrentState().getWMEs().size() == 4;
}

/// again an equality check -- with inputs 1234 and "1234".
bool const_numbers_are_used_as_strings()
{
    RuleParser p;
    Reasoner reasoner;

    auto rules = p.parseRules(
        // a bit bloated on purpose, to prevent node-reusage and actually have two
        // different makeSkolem nodes with different input triples
        "[r1: (<input1> <value> ?val), makeSkolem(?skolem ?val) -> (<s1> <value> ?skolem)]"
        "[r2: (<input2> <value> ?val), makeSkolem(?skolem ?val) -> (<s2> <value> ?skolem)]"
        // check if the created skolems are equal
        "[r3: (<s1> <value> ?a), (<s2> <value> ?a) -> (<skolems> <are> <equal>)]"
        // insert the needed data
        "[data: true() -> (<input1> <value> 1234), (<input2> <value> \"1234\")]",
        reasoner.net()
    );

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string(".dot"));

    // check if the equality-rule was triggered by counting WMEs:
    // - 2x (<input_> <value> 1234 / "1234")
    // - 2x (<s_> <value> ?skolem)
    // - 1x (<skolems> <are> <equal>)
    return reasoner.getCurrentState().getWMEs().size() == 5;
}



/// this is more of a usage example:
/// given value options for different parts of an object, create all possible
/// combinations
bool use_skolems_to_explicitely_create_combinations()
{
    RuleParser p;
    Reasoner reasoner;

    auto rules = p.parseRules(
        // the actual rule creating the combinations
        "["
            "(<Part1> <option> ?v1), (<Part2> <option> ?v2), (<Part3> <option> ?v3), "
            "makeSkolem(?c ?v1 ?v2 ?v3)"
            "->"
            "(?c <type> <combination>), (?c <p1> ?v1), (?c <p2> ?v2), (?c <p3> ?v3)"
        "]"
        // the data
        "["
            "data: true() -> "
            "(<Part1> <option> \"r\"), (<Part1> <option> \"g\"), (<Part1> <option> \"b\"),"
            "(<Part2> <option> \"r\"), (<Part2> <option> \"g\"), (<Part2> <option> \"b\"),"
            "(<Part3> <option> \"r\"), (<Part3> <option> \"g\"), (<Part3> <option> \"b\")"
        "]",
        reasoner.net()
    );

    reasoner.performInference();
    save(reasoner.net(), __func__ + std::string(".dot"));

    // check if the number of WMEs is as expected:
    // - 3x3 = 9x (<Part_> <option> "_")
    // - 3x3x3 = combinations x 4 triples: type, p1, p2, p3
    //         = 27 x 4
    //         = 108
    // - total: 117
    return reasoner.getCurrentState().getWMEs().size() == 117;
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
    TEST(skolem_from_const_string);
    TEST(skolem_from_const_number);
    TEST(two_skolems_with_same_arguments_are_equal);
    TEST(two_skolems_with_different_arguments_are_unequal);
    TEST(const_numbers_are_used_as_strings);
    TEST(use_skolems_to_explicitely_create_combinations);
    return failed;
}
