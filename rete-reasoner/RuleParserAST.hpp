#ifndef RETE_RULEPARSERAST_HPP_
#define RETE_RULEPARSERAST_HPP_

#define USE_RTTI
#include <pegmatite/pegmatite.hh>

#include <string>
#include <sstream>
#include <map>

#include <iostream>

/**
This file contains the class declarations from which an AST can be constructed. The actual
construction is done by the parserlib library and according to the rule grammar.
*/

namespace rete {
    namespace ast {

        namespace peg = pegmatite;

        class PrefixDefinition : public peg::ASTContainer {
        public:
            peg::ASTChild<peg::ASTString> name_, uri_;
        };

        /**
            Every argument in a precondition or effect, in a triple or whatever, is basically a
            string in the rule definition. Subclasses of Argument allow interpretation as something
            more specific, like variables, numbers, URIs, ...
        */
        class Argument : public peg::ASTString {
        public:
            virtual ~Argument() {}
            virtual bool isVariable() const { return false; }
            virtual bool isNumber() const { return false; }
            virtual bool isString() const { return false; }
            virtual bool isURI() const { return false; }

            virtual void substitutePrefixes(const std::map<std::string, std::string>&)
            {
                // nothing to do. only implemented by URIs.
            }

            virtual float toFloat() const
            {
                std::stringstream ss(*this);
                float tmp;
                ss >> tmp;
                return tmp;
            }
        };

        class Variable : public Argument {
            bool isVariable() const override { return true; }
        };

        class Number : public Argument {
            bool isNumber() const override { return true; }
        };

        class QuotedString : public Argument {
            bool isString() const override { return true; }
        };

        class URI : public Argument {
            bool isURI() const override { return true; }

            /**
                Replaces URI prefixes, e.g.:
                    { "rdf:" : "http://www.w3.org/.../foo#" }
                turns
                    "rdf:type"
                into
                    "<http://www.w3.org/.../foo#type>"

            */
            void substitutePrefixes(const std::map<std::string, std::string>& pairs) override
            {
                for (auto& p : pairs)
                {
                    const std::string& pre = p.first;
                    const std::string& subst = p.second;

                    if (this->size() < pre.size()) continue;

                    auto match = std::mismatch(pre.begin(), pre.end(), this->begin());
                    if (match.first == pre.end())
                    {
                        // prefix.first is prefix of this
                        this->replace(this->begin(), match.second, subst.begin(), subst.end());
                        // add <...>
                        this->insert(0, "<");
                        this->push_back('>');
                        break;
                    }
                }
            }
        };


        class Precondition : public peg::ASTContainer {
            bool noValue_;
        public:
            friend std::ostream& operator << (std::ostream&, Precondition&);

            std::string str_;

            /**
                An optional name. Triples don't have one (no name -> implicitly triple), but
                builtins need one.
            */
            peg::ASTPtr<peg::ASTString, true> name_;

            /**
                The list of arguments for this condition.
                For a triple this could be e.g. ["?a", "rdf:type", "?b"]
            */
            peg::ASTList<Argument> args_;

            /**
                A name for the type of precondition, used to decide how to parse it into a node
            */
            virtual std::string type() const
            {
                if (name_) return *name_;
                return "Triple";
            }

            bool isNoValue() { return noValue_; }

            bool construct(const peg::InputRange& r, peg::ASTStack& st, const peg::ErrorReporter& err)
            {
                str_ = r.str();
                noValue_ = false;

                auto str = r.str();
                std::cout << "constructing precondition from string: " << str << std::endl;

                // check for optional prefix
                const std::string negations[] = {"noValue ", "no ", "novalue " };

                for (auto& negation : negations)
                {
                    if (str.size() > negation.size())
                    {
                        auto it = std::mismatch(std::begin(negation), std::end(negation), std::begin(str));
                        if (it.first == std::end(negation))
                        {
                            std::cout << "precondition is negated!" << std::endl;
                            noValue_ = true;
                            break;
                        }
                    }
                }

                return peg::ASTContainer::construct(r, st, err);
            }
        };


        class Triple : public Precondition {
        public:
            const Argument& subject() const { return **args_.begin(); }
            const Argument& predicate() const { return **(++args_.begin()); }
            const Argument& object() const { return **(++(++args_.begin())); }
        };

        class Builtin : public Precondition {
        };

        class GenericAlphaCondition : public Precondition {
        };


        class Effect : public peg::ASTContainer {
        public:

            std::string str_;

            /**
                An optional name. InferTriple is shortcut, without a name.
            */
            peg::ASTPtr<peg::ASTString, true> name_;

            /**
                The list of arguments for the effect.
            */
            peg::ASTList<Argument> args_;

            /**
                A name for the type of effect.
            */
            virtual std::string type() const{
                if (name_) return *name_;
                return "Triple";
            }

            bool construct(const peg::InputRange& r, peg::ASTStack& st, const peg::ErrorReporter& err)
            {
                str_ = r.str();
                return this->peg::ASTContainer::construct(r, st, err);
            }

        };

        class InferTriple : public Effect {
        public:
            const Argument& subject() const { return **args_.begin(); }
            const Argument& predicate() const { return **(++args_.begin()); }
            const Argument& object() const { return **(++(++args_.begin())); }
        };

        class GenericEffect : public Effect {
        };


        class Rule : public peg::ASTContainer {
        public:
            std::string str_;

            peg::ASTPtr<peg::ASTString, true> name_;
            peg::ASTList<Precondition> conditions_;
            peg::ASTList<Effect> effects_;

            bool construct(const peg::InputRange& r, peg::ASTStack& st, const peg::ErrorReporter& err)
            {
                str_ = r.str();
                return this->peg::ASTContainer::construct(r, st, err);
            }
        };

        class Rules : public peg::ASTContainer {
        public:
            peg::ASTList<PrefixDefinition> prefixes_;
            peg::ASTList<Rule> rules_;
        };

    } /* ast */
} /* rete */


#endif /* end of include guard: RETE_RULEPARSERAST_HPP_ */
