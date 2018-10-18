#ifndef RETE_RULE_PARSER_HPP_
#define RETE_RULE_PARSER_HPP_

#include <string>
#include <rete-core/Network.hpp>

namespace rete {

/**
    Parses rules that are given in text format, e.g.
    [(?a rdf:subClassOf ?b), (?b rdf:subClassOf ?c) -> (?a rdf:subClassOf ?c)]
*/
class RuleParser {
public:
    /**
        Interprets the given string as a definition of rules and constructs them in the given
        network.
        \arg rules the string defining the rules
        \arg network the rete network to integrate the rules into
        \return true if parsing and construction of the rules was successfull
        TODO: throw an exception with more information when the rule syntax was violated and the parsing failed.
    */
    bool parseRules(const std::string& rules, Network& network);
};

} /* rete */

#endif /* end of include guard: RETE_RULE_PARSER_HPP_ */
