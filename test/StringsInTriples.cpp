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

bool containsTriple(Reasoner reasoner,
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

bool string_in_wme_matches_string_in_triple()
{
    RuleParser p;
    p.registerNodeBuilder<MutableNodeBuilder>();
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[MutableWME(?str), (<a> <foo> ?str) -> (<test> <is> <successfull>)]",
        reasoner.net()
    );

    auto wme = std::make_shared<MutableWME>();
    wme->value_ = "foo";
    auto triple = std::make_shared<Triple>("<a>", "<foo>", "\"foo\"");
    auto ev = std::make_shared<AssertedEvidence>("asserted");

    reasoner.addEvidence(wme, ev);
    reasoner.addEvidence(triple, ev);
    reasoner.performInference();

    save(reasoner.net(), __func__ + std::string(".dot"));

    return reasoner.getCurrentState().numWMEs() == 3;
}


bool part_in_triple_inferred_from_string_in_triple_stays_quoted_string()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[(<a> <foo> ?str) -> (<test> <foo> ?str)]",
        reasoner.net()
    );

    auto triple = std::make_shared<Triple>("<a>", "<foo>", "\"bar\"");
    auto ev = std::make_shared<AssertedEvidence>("asserted");

    reasoner.addEvidence(triple, ev);
    reasoner.performInference();

    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<test>", "<foo>", "\"bar\"");
}

bool part_in_triple_inferred_from_resource_in_triple_stays_resource()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[(<a> <foo> ?r) -> (<test> <foo> ?r)]",
        reasoner.net()
    );

    auto triple = std::make_shared<Triple>("<a>", "<foo>", "<bar>");
    auto ev = std::make_shared<AssertedEvidence>("asserted");

    reasoner.addEvidence(triple, ev);
    reasoner.performInference();

    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<test>", "<foo>", "<bar>");
}

bool part_in_triple_inferred_from_constant_string_is_quoted()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[true() -> (<test> <foo> \"bar\")]",
        reasoner.net()
    );

    reasoner.performInference();

    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<test>", "<foo>", "\"bar\"");
}

bool part_in_triple_inferred_from_string_in_wme_is_quoted()
{
    RuleParser p;
    p.registerNodeBuilder<MutableNodeBuilder>();
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[MutableWME(?str) -> (<test> <foo> ?str)]",
        reasoner.net()
    );

    auto wme = std::make_shared<MutableWME>();
    wme->value_ = "foo";
    auto ev = std::make_shared<AssertedEvidence>("asserted");

    reasoner.addEvidence(wme, ev);
    reasoner.performInference();

    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<test>", "<foo>", "\"foo\"");
}

/**
    This one could be a bit unintuitive, that's why I included it here:
    Just because something looks like a reasource (e.g. <sempr:Something_1>),
    it is not interpreted as one. The interpretations that are used are those
    which are explicitely allowed by the accessors that implement them. And
    since the MutableWME is not supposed to hold resources, its content is not
    interpreted as such -- it only allows access to strings, so strings they
    are! The other tests in which resources from triples need to stay resources
    make use of the fact that the triple-accessor has an interpretation of its
    contents as raw rdf-stuff. Even there, the actual content is not inspected!
    Only the registered data types!
*/
bool part_in_triple_inferred_from_string_that_looks_like_a_resource_is_still_a_string_and_quoted()
{
    RuleParser p;
    p.registerNodeBuilder<MutableNodeBuilder>();
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[MutableWME(?str) -> (<test> <foo> ?str)]",
        reasoner.net()
    );

    auto wme = std::make_shared<MutableWME>();
    wme->value_ = "<foo>";
    auto ev = std::make_shared<AssertedEvidence>("asserted");

    reasoner.addEvidence(wme, ev);
    reasoner.performInference();

    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<test>", "<foo>", "\"<foo>\"");

}


// yes, this is no longer just about strings in triples... but fits the same
// problem/topic
bool constant_number_in_triple_stays_unquoted()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[true() -> (<test> <foo> 42)]",
        reasoner.net()
    );

    reasoner.performInference();

    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<test>", "<foo>", "42");
}

bool computed_number_in_triple_is_not_quoted()
{
    RuleParser p;
    Reasoner reasoner;
    auto rules = p.parseRules(
        "[true(), sum(?sum 21 21) -> (<test> <foo> ?sum)]",
        reasoner.net()
    );

    reasoner.performInference();

    save(reasoner.net(), __func__ + std::string(".dot"));

    return containsTriple(reasoner, "<test>", "<foo>", std::to_string(42));
}


/**
    ToTripleConversion allows to convert strings and numbers to TriplePart,
    but this conversion does not make sense in the subject or predicate position
    of a triple. Hence, the TripleEffectBuilder should throw an exception when
    it gets an accessor for sub/pred that does not support TriplePart right
    away.
*/
bool computed_number_in_inferred_triple_subject_throws()
{
    RuleParser p;
    Reasoner reasoner;
    try {
        auto rules = p.parseRules(
            "[true(), sum(?sum 21 21) -> (?sum <foo> <bar>)]",
            reasoner.net()
        );
        return false;
    } catch (NodeBuilderException& e) {
        return true;
    }
}


// TODO: More reading / matching tests
// TODO: Test to INFER triples from different sources

/*
#define TEST(function) \
    if (!((function))()) \
    { \
        std::cout << "failed test: " << #function << std::endl; \
        failed++; \
    }
*/
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
    TEST(string_in_triple_matches_constant_string);
    TEST(string_in_triple_matches_string_in_triple);
    TEST(string_in_wme_matches_constant_string);
    TEST(string_in_wme_matches_string_in_triple);
    TEST(part_in_triple_inferred_from_string_in_triple_stays_quoted_string);
    TEST(part_in_triple_inferred_from_resource_in_triple_stays_resource);
    TEST(part_in_triple_inferred_from_constant_string_is_quoted);
    TEST(part_in_triple_inferred_from_string_in_wme_is_quoted);
    TEST(part_in_triple_inferred_from_string_that_looks_like_a_resource_is_still_a_string_and_quoted);
    TEST(constant_number_in_triple_stays_unquoted);
    TEST(computed_number_in_triple_is_not_quoted);
    TEST(computed_number_in_inferred_triple_subject_throws);

    return failed;
}
