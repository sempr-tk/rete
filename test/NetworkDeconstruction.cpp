#include <iostream>
#include <fstream>

#include "../rete-core/ReteCore.hpp"
#include "../rete-rdf/ReteRDF.hpp"

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/AssertedEvidence.hpp"

#include "../rete-reasoner/RuleParser.hpp"

using namespace rete;


void save(Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}


/**
    This test only checks if the network gets deconstructed properly, i.e. nodes are removed
    when all their children are gone. It's a test if the pointer-structure was implemented
    correctly, but does **not** check if activations are retracted. This is done in
    "RemovingRules.cpp".
*/
int main()
{
    Reasoner reasoner;
    RuleParser parser;

    auto rules = parser.parseRules(
        "[rule1: (?a <foo> ?b), (?b <foo> ?c) ->                (<just> <a> <dummy>)]"
        "[rule2: (?a <foo> ?b), (?b <foo> ?c), (?c <foo> ?d) -> (<just> <a> <dummy>)]"
        "[rule3:                (?b <foo> ?c), (?c <bar> ?d) -> (<just> <a> <dummy>)]"
        "[rule4:                (?c <bar> <baz>) ->             (<just> <a> <dummy>)]"
        ,
        reasoner.net()
    );

    save(reasoner.net(), "deconstruct_0.dot");

    // convenience method to remove rules
    auto deconstruct = [&](const std::string& name)
    {
        auto it = std::remove_if(rules.begin(), rules.end(),
                [name](rete::ParsedRule::Ptr& rule) -> bool
                {
                    return rule->name() == name;
                });
        rules.erase(it, rules.end());
    };


    for (auto r : rules)
    {
        std::cout << r->name() << std::endl;
    }

    deconstruct("rule2");
    save(reasoner.net(), "deconstruct_1.dot");

    deconstruct("rule3");
    save(reasoner.net(), "deconstruct_2.dot");

    deconstruct("rule1");
    save(reasoner.net(), "deconstruct_3.dot");

    deconstruct("rule4");
    save(reasoner.net(), "deconstruct_4.dot");


    // only the retes root node and its alpha memory should remain
    auto root = reasoner.net().getRoot();
    auto rootmem = root->getAlphaMemory();

    std::vector<AlphaNode::Ptr> rootChildren;
    root->getChildren(rootChildren);

    std::vector<BetaNode::Ptr> rootmemChildren;
    rootmem->getChildren(rootmemChildren);

    if (!rootChildren.empty()) return 1;
    if (!rootmemChildren.empty()) return 2;

    return 0;
}
