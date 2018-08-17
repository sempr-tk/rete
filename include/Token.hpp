#ifndef RETE_TOKEN_HPP
#define RETE_TOKEN_HPP

#include <memory>
#include <string>

#include "WME.hpp"

namespace rete {

/**
    Tokens are basically a list of WMEs that satisfy the first part of the conditions. When
    propagated fully through the network and reaching the production nodes, the tokens are the
    matching results, on which a production rule can be executed.
*/
struct Token {
    using Ptr = std::shared_ptr<Token>;

    Token::Ptr parent;
    WME::Ptr wme;

    std::string toString() const;


    /**
        returns true if all WMEs in the two token match -- they are equal (but not necessarily the
        same instances).
        This is needed to correctly update the agenda when adding and removing tokens, to check if
        a new token matches one that were to be retracted previously.
    */
    bool equals(const Token& other) const;
};

} /* rete */

#endif /* end of include guard: RETE_TOKEN_HPP */
