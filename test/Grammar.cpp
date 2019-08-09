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

    bool ok;
    // test quoted strings
    ok = p.parseRules(
        "[rule1: (<a> <b> <c>) -> (<a> <b> \"Hello, world!\")]\n",
        reasoner.net()
    );

    if (!ok) return 1;
    
    // test underscores in prefixed URIs
    ok = p.parseRules(
        "[rule1: (<a> <b> foo:Something_1) -> (<a> <b> <c>)]\n",
        reasoner.net()
    );

    if (!ok) return 2;

    return 0;
}
