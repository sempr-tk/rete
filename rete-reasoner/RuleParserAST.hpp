#ifndef RETE_RULEPARSERAST_HPP_
#define RETE_RULEPARSERAST_HPP_

#define USE_RTTI
#include <pegmatite/pegmatite.hh>

#include <string>
#include <sstream>
#include <map>

#include <iostream>
#include <iomanip>

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
            virtual bool isInt() const { return false; }
            virtual bool isFloat() const { return false; }

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

            virtual int toInt() const
            {
                std::stringstream ss(*this);
                int tmp;
                ss >> tmp;
                return tmp;
            }


            /**
                Converts the value to std::string. In most cases this just
                copies the argument as it was specified in the rule, but
                it allows e.g. the URI subclass return a modified value.
            */
            virtual std::string toString() const
            {
                return *this;
            }
        };

        class Variable : public Argument {
            bool isVariable() const override { return true; }
        };

        class Number : public Argument {
            bool isNumber() const override { return true; }
        };

        class Int : public Number {
            bool isInt() const override { return true; }
        };

        class Float : public Number {
            bool isFloat() const override { return true; }
        };

        class QuotedString : public Argument {
            bool isString() const override { return true; }

        public:
            /**
                During construction, unquote the string!
            */
            bool construct(const peg::InputRange& r, peg::ASTStack& st, const peg::ErrorReporter& err) override
            {
                // first construct the argument
                bool ok = Argument::construct(r, st, err);
                // then update this (which is a std::string!) by removing quotes
                // and escape chars.
                //     "Hello \"friend\"..."
                // becomes
                //     Hello "friend"...
                std::stringstream ss(r.str());
                ss >> std::quoted(*this);

                return ok;
            }

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

            /**
                Returns the URI without the angular brackets.
            */
            std::string toString() const override
            {
                std::string result = *this;
                if (*result.begin() == '<' && *result.rbegin() == '>')
                {
                    result = result.substr(1, result.size()-2);
                }
                return result;
            }
        };

        /**
            Base class for preconditions, without any ast members.
            This is done to provide an interface that allows to check if a
            precondition is either a primitive precondition, with a name and a
            list of arguments, or a noValueGroup with a list of primitives
            (which all may be noValueGroups or primitives, too).
        */
        class PreconditionBase : public peg::ASTContainer {
        public:
            std::string str_;

            virtual ~PreconditionBase() {}
            virtual bool isNoValueGroup() const = 0;
            virtual bool isGroupBy() const = 0;
            virtual bool isPrimitive() const = 0;

            /**
                Calls substitutePrefixes on every argument of this condition
            */
            virtual void substituteArgumentPrefixes(const std::map<std::string, std::string>&)
            {
            }

            bool construct(const peg::InputRange& r, peg::ASTStack& st, const peg::ErrorReporter& err) override
            {
                str_ = r.str();
                return peg::ASTContainer::construct(r, st, err);
            }
        };

        class Precondition : public PreconditionBase {
        public:
            friend std::ostream& operator << (std::ostream&, Precondition&);


            bool isNoValueGroup() const override { return false; }
            bool isGroupBy() const override { return false; }
            bool isPrimitive() const override { return true; }

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

            void substituteArgumentPrefixes(const std::map<std::string, std::string>& pairs) override
            {
                for (auto& arg : args_)
                {
                    arg->substitutePrefixes(pairs);
                }
            }

            /**
                A name for the type of precondition, used to decide how to parse it into a node
            */
            virtual std::string type() const
            {
                if (name_) return *name_;
                return "Triple";
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


        class NoValueGroup : public PreconditionBase {
        public:
            peg::ASTList<PreconditionBase> conditions_;

            bool isPrimitive() const override { return false; }
            bool isNoValueGroup() const override { return true; }
            bool isGroupBy() const override { return false; }

            void substituteArgumentPrefixes(const std::map<std::string, std::string>& pairs) override
            {
                for (auto& condition : conditions_)
                {
                    condition->substituteArgumentPrefixes(pairs);
                }
            }
        };


        class GroupBy : public PreconditionBase {
        public:
            peg::ASTList<Variable> variables_;

            bool isPrimitive() const override { return false; }
            bool isNoValueGroup() const override { return false; }
            bool isGroupBy() const override { return true; }
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
            peg::ASTList<PreconditionBase> conditions_;
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
