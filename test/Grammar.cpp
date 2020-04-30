#include <iostream>
#include <fstream>

#include "../rete-reasoner/Reasoner.hpp"
#include "../rete-reasoner/RuleParser.hpp"
#include "../rete-reasoner/Exceptions.hpp"

using namespace rete;


void save(Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}


int main()
{
    RuleParser p;
    Reasoner reasoner;

    // test quoted strings
    auto rules = p.parseRules(
        "[rule1: (<a> <b> <c>) -> (<a> <b> \"Hello, world!\")]\n",
        reasoner.net()
    );

    // test underscores in prefixed URIs
    auto moreRules = p.parseRules(
        "[rule1: (<a> <b> foo:Something_1) -> (<a> <b> <c>)]\n",
        reasoner.net()
    );

    return 0;
}
