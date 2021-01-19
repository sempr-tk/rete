#ifndef RETE_RULEPARSERAST_HPP_
#define RETE_RULEPARSERAST_HPP_

#define USE_RTTI
#include <pegmatite/pegmatite.hh>
#include "Exceptions.hpp"

#include <string>
#include <sstream>
#include <map>

#include <iostream>
#include <iomanip>


// helper def for an easy setup of clone methods.
// ASTNodes cannot be copied directly (ASTNode(const ASTNode&) = delete;),
// but the copy assignment operator is defaulted...
// So I can copy? Is this a hack? A bug? Probably. Do I care? Yes.
// Do I hack it anyway? ....
#define CLONE_METHOD(classname) \
    classname* clone() const override\
    {\
        classname* arg = new classname();\
        *arg = *this;\
        return arg;\
    }



/**
This file contains the class declarations from which an AST can be constructed. The actual
construction is done by the parserlib library and according to the rule grammar.
*/

namespace rete {
    namespace ast {

        namespace peg = pegmatite;

        class PrefixDefinition : public peg::ASTContainer {
        public:
            peg::ASTPtr<peg::ASTString, true> overrideFlag_; // optional
            peg::ASTChild<peg::ASTString> name_, uri_;

            bool isOverride() const
            {
                return overrideFlag_ != nullptr;
            }

            PrefixDefinition* clone() const
            {
                PrefixDefinition* arg = new PrefixDefinition();
                arg->name_ = name_;
                arg->uri_ = uri_;
                if (overrideFlag_)
                {
                    arg->overrideFlag_.reset(new peg::ASTString());
                    *arg->overrideFlag_ = *overrideFlag_;
                }
                return arg;
            }
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
            virtual bool isGlobalConstRef() const { return false; }

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

            virtual Argument* clone() const
            {
                // ASTNode(const ASTNode&) = delete; aber
                // ASTNode::operator = (const ASTNode&) = default;
                // ... hacky?
                Argument* arg = new Argument();
                *arg = *this;
                return arg;
            }
        };

        class Variable : public Argument {
            bool isVariable() const override { return true; }
        public:
            CLONE_METHOD(Variable);
        };

        class Number : public Argument {
            bool isNumber() const override { return true; }
        public:
            CLONE_METHOD(Number);
        };

        class Int : public Number {
            bool isInt() const override { return true; }
        public:
            CLONE_METHOD(Int);
        };

        class Float : public Number {
            bool isFloat() const override { return true; }
        public:
            CLONE_METHOD(Float);
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

            CLONE_METHOD(QuotedString);
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

        public:
            CLONE_METHOD(URI)
        };


        /**
            A GlobalConstantReference is an argument which refers to a
            previously defined global constant, and must thus be replaced by
            that.
        */
        class GlobalConstantReference : public Argument {
        public:
            bool isGlobalConstRef() const override { return true; }
            CLONE_METHOD(GlobalConstantReference)
        };

        /**
            A GlobalConstantDefinition defines a global constant.
            It is a mapping between an identifier and another argument.
        */
        class GlobalConstantDefinition : public peg::ASTContainer {
        public:
            peg::ASTPtr<peg::ASTString, true> overrideFlag_;
            peg::ASTChild<peg::ASTString> id_;
            peg::ASTPtr<Argument, false> value_;

            bool isOverride() const
            {
                return overrideFlag_ != nullptr;
            }

            GlobalConstantDefinition* clone() const
            {
                GlobalConstantDefinition* arg = new GlobalConstantDefinition();
                arg->id_ = id_;
                arg->value_.reset(value_->clone());
                if (overrideFlag_)
                {
                    arg->overrideFlag_.reset(new peg::ASTString());
                    *arg->overrideFlag_ = *overrideFlag_;
                }
                return arg;
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

            /**
                Replaces references to global constants by the actual values
            */
            virtual void replaceGlobalConstantReferences(
                    const peg::ASTList<GlobalConstantDefinition>&)
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

            virtual void replaceGlobalConstantReferences(
                    const peg::ASTList<GlobalConstantDefinition>& defs) override
            {
                for (auto& arg : args_)
                {
                    if (arg->isGlobalConstRef())
                    {
                        bool found = false;
                        for (auto& def : defs)
                        {
                            if(*arg == def->id_)
                            {
                                arg.reset(def->value_->clone());
                                found = true;
                                break;
                            }
                        }
                        if (!found)
                        {
                            throw std::exception();
                        }
                    }
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

            virtual void replaceGlobalConstantReferences(
                    const peg::ASTList<GlobalConstantDefinition>& defs) override
            {
                for (auto& condition : conditions_)
                {
                    condition->replaceGlobalConstantReferences(defs);
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

            bool construct(const peg::InputRange& r, peg::ASTStack& st, const peg::ErrorReporter& err) override
            {
                str_ = r.str();
                return this->peg::ASTContainer::construct(r, st, err);
            }

            virtual void replaceGlobalConstantReferences(
                    const peg::ASTList<GlobalConstantDefinition>& defs)
            {
                for (auto& arg : args_)
                {
                    if (arg->isGlobalConstRef())
                    {
                        for (auto& def : defs)
                        {
                            if(*arg == def->id_)
                            {
                                arg = std::unique_ptr<Argument>(def->value_->clone());
                            }
                        }
                    }
                }
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
            peg::ASTList<GlobalConstantDefinition> constants_;

            /*
                A "Rules" node consists either of just a bunch of simple
                "Rule" nodes, or may contain "Rules" nodes itself. These
                scoped rules are supposed to get access to all the parents
                PrefixDefinitions and GlobalConstantDefinitions, too.
            */
            peg::ASTList<Rule> rules_;
            peg::ASTList<Rules> scopedRules_;

            /**
                Default implementation of ASTContainer constructs the members
                one after the other, in reverse order. This means that the
                elements must be sorted as they members of this class; but the
                grammar allows a wild mix of rules and scopedRules.
                To accommodate for that, the construct method is overridden to
                parse rules and scopedRules as long as it changes the stack.
            */
            bool construct(const peg::InputRange& r, peg::ASTStack& st, const peg::ErrorReporter& err)
            {
                bool unchanged = false;
                while (!unchanged) {
                    size_t stackSizeBefore = st.size();
                    scopedRules_.construct(r, st, err);
                    rules_.construct(r, st, err);
                    unchanged = (st.size() == stackSizeBefore);
                }

                constants_.construct(r, st, err);
                prefixes_.construct(r, st, err);
                return true;
            }


            /**
                Makes the definitions of the parent (@PREFIX, $value)
                available to the nested rules.
            */
            void propagateDefinitionsToChildren()
            {
                // collect prefix substs in map
                std::map<std::string, std::string> prefixReplacements;
                for (auto& pre : prefixes_)
                {
                    const std::string& uri = pre->uri_;
                    prefixReplacements[pre->name_ + ":"] = uri.substr(1, uri.size()-2); // trim <>
                }

                // apply the prefix defs to the constant definitions which are
                // already present first. This is needed because otherwise the
                // definition
                //      @PREFIX foo: <foo#>
                //      $value : foo:a
                // would bind $value to "foo:a" and insert it into all places
                // where "$value" is used, such that a _later_ redefinition of
                // "foo:" could change it. But it is more intuitive (I guess)
                // to bind $value to <foo#a>, and only change the $value if it
                // is explicitely overridden.
                for (auto& gconst : constants_)
                {
                    gconst->value_->substitutePrefixes(prefixReplacements);
                }


                for (auto& child : scopedRules_)
                {
                    // copy prefixdefinitions to child rules nodes
                    for (auto& prefixdef : prefixes_)
                    {
                        auto it = std::find_if(
                                child->prefixes_.begin(),
                                child->prefixes_.end(),
                                [&prefixdef](auto& pdef) -> bool
                                {
                                    return pdef->name_ == prefixdef->name_;
                                });
                        if (it != child->prefixes_.end())
                        {
                            if (!(*it)->isOverride())
                                throw ParserException("Overriding previous definition of @PREFIX " + (*it)->name_);
                        }
                        else
                        {
                            // only use the parents definition if the child does not define its own
                            std::unique_ptr<PrefixDefinition> ptr(prefixdef->clone());
                            child->prefixes_.push_back(std::move(ptr));
                        }
                    }

                    // copy global constants to child nodes
                    for (auto& globalConst : constants_)
                    {
                        auto it = std::find_if(
                                child->constants_.begin(),
                                child->constants_.end(),
                                [&globalConst](auto& gconst) -> bool
                                {
                                    return gconst->id_ == globalConst->id_;
                                });
                        if (it != child->constants_.end())
                        {
                            if (!(*it)->isOverride())
                                throw ParserException("Overriding previous definition of " + (*it)->id_);
                        }
                        else
                        {
                            std::unique_ptr<GlobalConstantDefinition> ptr(globalConst->clone());
                            child->constants_.push_back(std::move(ptr));
                        }
                    }

                    // propagate further down:
                    child->propagateDefinitionsToChildren();

                    // TODO: test case
                    // TODO: override flag for prefixdefs and constants to suppress warning/error
                }
            }


            /**
                Applies the prefix and constants definitions to the contained
                rules. Also calls this method on nested Rules-nodes.
            */
            void applyPrefixesAndGlobalConstantsDefinitionsToRules()
            {
                // collect prefix substs in map
                std::map<std::string, std::string> prefixReplacements;
                for (auto& pre : prefixes_)
                {
                    const std::string& uri = pre->uri_;
                    prefixReplacements[pre->name_ + ":"] = uri.substr(1, uri.size()-2); // trim <>
                }

                // for all rules: apply prefix substs and global consts to all
                // conditions and effects
                for (auto& rule : rules_)
                {
                    for (auto& condition : rule->conditions_)
                    {
                        condition->replaceGlobalConstantReferences(constants_);
                        condition->substituteArgumentPrefixes(prefixReplacements);
                    }

                    for (auto& effect : rule->effects_)
                    {
                        effect->replaceGlobalConstantReferences(constants_);
                        for (auto& arg : effect->args_)
                        {
                            arg->substitutePrefixes(prefixReplacements);
                        }
                    }
                }

                // for all nested scopes: trigger this process
                for (auto& nested : scopedRules_)
                {
                    nested->applyPrefixesAndGlobalConstantsDefinitionsToRules();
                }
            }
        };

    } /* ast */
} /* rete */

#undef CLONE_METHOD

#endif /* end of include guard: RETE_RULEPARSERAST_HPP_ */
