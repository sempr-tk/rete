#ifndef RETE_RULEPARSERAST_HPP_
#define RETE_RULEPARSERAST_HPP_

#define USE_RTTI
#include <pegmatite/pegmatite.hh>

#include <string>
#include <map>

/**
This file contains the class declarations from which an AST can be constructed. The actual
construction is done by the parserlib library and according to the rule grammar.
*/

namespace rete {
    namespace ast {

        namespace peg = pegmatite;

        class PrefixDefinition : public peg::ASTContainer {
        public:
            peg::ASTPtr<peg::ASTString> name_, uri_;
        };

        class TripleElement : public peg::ASTContainer {
        public:
            peg::ASTPtr<peg::ASTString> value_;
            bool isVariable() const
            {
                return value_->size() > 1 && value_->at(0) == '?';
            }
        };


        class Triple : public peg::ASTContainer {
        public:
            peg::ASTPtr<TripleElement> subject_, predicate_, object_;

            friend std::ostream& operator << (std::ostream&, Triple&);

            /**
            Given a map[?key] = ?value,
            for every ?string that starts with ?key, replaces the ?key with ?value and adds '<' '>'
            around it, e.g.:
            map["rdf:"] = "http://www.w3.org/...#"
            rdf:type
            -->
            <http:://www.w3.org/...#type>
            */
            void substitutePrefixes(std::map<std::string, std::string>& prefixes);
        };


        class Triples : public peg::ASTContainer {
        public:
            peg::ASTList<Triple> triples_;
        };

        class Rule : public peg::ASTContainer {
        public:
            peg::ASTPtr<peg::ASTString, true> name_;
            peg::ASTPtr<Triples> conditions_, effects_;
        };

        class Rules : public peg::ASTContainer {
        public:
            peg::ASTList<PrefixDefinition> prefixes_;
            peg::ASTList<Rule> rules_;
        };

    } /* ast */
} /* rete */


#endif /* end of include guard: RETE_RULEPARSERAST_HPP_ */
