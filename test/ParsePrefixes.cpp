#include <iostream>
#include <fstream>

#include "../rete-reasoner/Reasoner.hpp"
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

    #ifdef DEBUG_PARSING
    std::cout << "DEBUG PARSING" << std::endl;
    #endif

    RuleParser p;
    Reasoner reasoner;

    /**
        Check if prefixes are parsed / accepted at all.
        TODO: Automatically check if the prefixes are used correctly? How? Not worth the trouble, see prefixes.dot.
    */
    bool ok = p.parseRules(
        "@PREFIX rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#>\n"
        "@PREFIX sempr: <sempr:>\n"
        "[(?o rdf:type sempr:SpatialObject), (?o sempr:on ?other) -> (<just> rdf:a <test>)]\n",
        reasoner.net()
    );

    save(reasoner.net(), "prefixes.dot");

    return (ok ? 0 : 1);
}
