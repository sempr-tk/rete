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

int main()
{
    Reasoner reasoner;
    RuleParser parser;

    parser.parseRules(
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
        auto ps = reasoner.net().getProductions();
        for (auto p : ps)
        {
            if (p->getName() == name)
            {
                reasoner.net().removeProduction(p);
                return;
            }
        }
    };


    for (auto p : reasoner.net().getProductions())
    {
        std::cout << p->getName() << std::endl;
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
