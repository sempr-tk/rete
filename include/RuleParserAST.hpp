#ifndef RETE_RULEPARSERAST_HPP_
#define RETE_RULEPARSERAST_HPP_

// #include <parserlib.hpp>
#include "../parserlib/source/parserlib.hpp"

#include <string>

/**
    This file contains the class declarations from which an AST can be constructed. The actual
    construction is done by the parserlib library and according to the rule grammar.
*/

namespace rete {
namespace ast {

namespace pl = parserlib;

class String : public pl::ast_container {
public:
    std::string value_;

    void construct(pl::ast_stack&) override
    {
        /**
            The pl::ast_container has member variables defining which part of the string we are
            parsing we currently inspect, so we use it here to save this part for convenience.
        */
        value_.insert(value_.begin(), m_begin.m_it, m_end.m_it);
    }
};

class TripleElement : public String {
public:
    bool isVariable() const
    {
        return value_.size() > 1 && value_[0] == '?';
    }
};


class Triple : public pl::ast_container {
public:
    pl::ast_ptr<TripleElement> subject_, predicate_, object_;

    friend std::ostream& operator << (std::ostream&, Triple&);
};

class Triples : public pl::ast_container {
public:
    pl::ast_list<Triple> triples_;
};

class Rule : public pl::ast_container {
public:
    pl::ast_ptr<Triples> conditions_, effects_;
};

class Rules : public pl::ast_container {
public:
    pl::ast_list<Rule> rules_;
};

} /* ast */
} /* rete */


#endif /* end of include guard: RETE_RULEPARSERAST_HPP_ */
