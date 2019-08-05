#include <iostream>
#include <fstream>
#include <sstream>

#include "../../rete-reasoner/Reasoner.hpp"
#include "../../rete-reasoner/RuleParser.hpp"


using namespace rete;


void save(Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}

int main(int argc, char** args)
{
    if (argc < 2)
    {
        std::cout << "Wrong number of arguments!" << std::endl;
        std::cout << "Usage: " << args[0] << " <file> [files ...] " << std::endl;
    }

    for (int i = 1; i < argc; i++)
    {

        RuleParser p;
        Reasoner reasoner;

        try
        {
            std::ifstream in(args[i]);
            std::stringstream buffer;
            buffer << in.rdbuf();

            p.parseRules(buffer.str(), reasoner.net());

            save(reasoner.net(), std::string(args[i]) + ".dot");
        }
        catch (const std::exception& exception)
        {
            std::cerr << exception.what() << std::endl;
        }
    }


    return 0;
}
