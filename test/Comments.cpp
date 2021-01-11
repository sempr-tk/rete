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
    RuleParser p;
    Reasoner reasoner;

    auto rules = p.parseRules(
        "# comment1a\n"
        "# comment1b\n"
        "@PREFIX rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#>\n"
        "# comment between prefix-defs\n"
        "@PREFIX sempr: <sempr:>\n"
        "# comment after prefix-defs\n"
        "# comment before global const defs\n"
        "$foo : 42 # comment in line with global const def\n"
        "# comment after global const def\n"
        "\n"
        "# comment before rule\n"
        "[name: #comment in line with rule name\n"
        "       (<sub> <pred> \"not-a-comment\"), #comment in line with triple\n"
        "       (<sub2> # comment in line with subject\n"
        "        <pred2> <obj2>)\n"
        " #comment before  arrow\n"
        " -> #comment in line with arrow\n"
        " # comment after arrow\n"
        " (<foo> <bar> <baz> #some inline comment with effect again\n"
        " )]\n"
        "# comment after rule\n"
        "[true() -> (<a> <b> <c>)]",

        reasoner.net()
    );

    save(reasoner.net(), "comments.dot");

    return 0; // if parsing failes an exception is thrown
}
