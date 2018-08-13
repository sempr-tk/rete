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

};

} /* rete */

#endif /* end of include guard: RETE_TOKEN_HPP */
