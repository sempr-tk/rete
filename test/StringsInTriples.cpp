#include <iostream>
#include <fstream>

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-reasoner/Exceptions.hpp"
#include "../rete-rdf/Triple.hpp"
#include "../rete-core/TupleWME.hpp"


#include "MutableWME.hpp"
using namespace rete;


void save(Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}


/**
    These tests serve to check if string handling in triples is done properly.
    Previously, there were two bugs which negated each other:
        - The contents of a triple were accessed raw, i.e. accessing the
          object of a triple (<a> <foo> "Hello, World!") would return
          "Hello, World!", including the quotation marks.
        - When parsing string constants in rules, the quotation marks were
          included in the value.
    So, the next thing on my todo list is fix this issue, by actually parsing
    the contents of a triple [*] (partly, I won't add full RDF support here to
    keep the dependencies at a minimum) and remove the quotes from string
    constants. But to make sure I get this right, I add these tests first.

    [*] This won't be perfect. I think my approach will allow to interpret
    parts of a triple as either "RawTripleContent", "std::string", "float" and
    "int", where the string interpretation will remove enclosing quotation marks
    or angle brackets, and the float/int interpretation parse the result through
    a stringstream.
*/

bool string_in_triple_matches_constant_string()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[(?a ?b \"foo\") -> (<test> <is> <successfull>)]",
        reasoner.net()
    );

    // note: a string in a triple is quoted!
    auto triple = std::make_shared<Triple>("<a>", "<b>", "\"foo\"");
    auto ev = std::make_shared<AssertedEvidence>("asserted");

    reasoner.addEvidence(triple, ev);
    reasoner.performInference();

    save(reasoner.net(), __func__ + std::string(".dot"));

    // expect two wmes: the added, and the inferred.
    return reasoner.getCurrentState().numWMEs() == 2;
}

bool string_in_triple_matches_string_in_triple()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[(<a> <foo> ?b), (<b> <foo> ?b) -> (<test> <is> <successfull>)]",
        reasoner.net()
    );

    // note: a string in a triple is quoted!
    auto triple1 = std::make_shared<Triple>("<a>", "<foo>", "\"baz\"");
    auto triple2 = std::make_shared<Triple>("<b>", "<foo>", "\"baz\"");
    auto ev = std::make_shared<AssertedEvidence>("asserted");

    reasoner.addEvidence(triple1, ev);
    reasoner.addEvidence(triple2, ev);
    reasoner.performInference();

    save(reasoner.net(), __func__ + std::string(".dot"));

    return reasoner.getCurrentState().numWMEs() == 3;
}

bool string_in_wme_matches_constant_string()
{
    RuleParser p;
    p.registerNodeBuilder<MutableNodeBuilder>();
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[MutableWME(\"foo\") -> (<test> <is> <successfull>)]",
        reasoner.net()
    );

    // note: a string in a triple is quoted!
    auto wme = std::make_shared<MutableWME>();
    wme->value_ = "foo";
    auto ev = std::make_shared<AssertedEvidence>("asserted");

    reasoner.addEvidence(wme, ev);
    reasoner.performInference();

    save(reasoner.net(), __func__ + std::string(".dot"));

    return reasoner.getCurrentState().numWMEs() == 2;
}

// TODO: More reading / matching tests
// TODO: Test to INFER triples from different sources


#define TEST(function) \
    if (!((function))()) \
    { \
        std::cout << "failed test: " << #function << std::endl; \
        failed++; \
    }


int main()
{
    int failed = 0;
    TEST(string_in_triple_matches_constant_string);
    TEST(string_in_triple_matches_string_in_triple);
    TEST(string_in_wme_matches_constant_string);

    return failed;
}
