#include "TripleConditionBuilder.hpp"
#include "../rete-rdf/TripleTypeAlpha.hpp"
#include "Exceptions.hpp"
#include <iostream>

namespace rete {

// helper to deal with "QuotedString" as well as string-literals (which are just
// "Argument"s so far)
std::string argToStr(const ast::Argument& arg)
{
    if (arg.isString())
    {
        // .isString -> this is a "QuotedString" argument, which...
        // is not quoted. The quotes have been removed during parsing,
        // but we need them here, to allow "QuotedString" in triple stuff,
        // which.. the grammar... doesn't allow...
        // Wow. Hacks on so many levels.
        std::stringstream ss;
        ss << std::quoted(arg);
        std::cout << ss.str() << std::endl;
        return ss.str();
    }
    else
    {
        std::cout << typeid(arg).name() << " not a quoted string..." << std::endl;
        return arg;
    }
}


TripleConditionBuilder::TripleConditionBuilder()
    : NodeBuilder("Triple", BuilderType::ALPHA)
{
}

void TripleConditionBuilder::buildAlpha(ArgumentList& args, std::vector<AlphaNode::Ptr>& nodes) const
{
#ifdef RETE_PARSER_VERBOSE
    std::cout << "TripleBuilder with args:" << std::endl;
    for (auto& arg : args)
    {
        std::cout << "  " << arg.getAST() << std::endl;
    }
#endif

    // should never happen, since the parser knows that triples need three arguments
    if (args.size() != 3) throw NodeBuilderException("Wrong number of arguments");

    // alpha nodes/conditions never get a bound variable.
    assert(args[0].getAccessor().get() == nullptr);
    assert(args[1].getAccessor().get() == nullptr);
    assert(args[2].getAccessor().get() == nullptr);

    // first: a type check. Is the wme even a triple?
    TripleTypeAlpha::Ptr typeCheckNode(new TripleTypeAlpha());
    nodes.push_back(typeCheckNode);


    // static checks for non variable parts of the triple.
    if (args[0].isConst())
    {
        TripleAlpha::Ptr alpha(new TripleAlpha(Triple::SUBJECT, argToStr(args[0].getAST())));
        nodes.push_back(alpha);

    }
    // if its variable, it will be bound after these checks.
    else
    {
        TripleAccessor::Ptr access(new TripleAccessor(Triple::SUBJECT));
        args[0].bind(access);
    }

    // same for predicate
    if (args[1].isConst())
    {
        TripleAlpha::Ptr alpha(new TripleAlpha(Triple::PREDICATE, argToStr(args[1].getAST())));
        nodes.push_back(alpha);
    }
    else
    {
        TripleAccessor::Ptr access(new TripleAccessor(Triple::PREDICATE));
        args[1].bind(access);
    }

    // same for object
    if (args[2].isConst())
    {
        TripleAlpha::Ptr alpha(new TripleAlpha(Triple::OBJECT, argToStr(args[2].getAST())));
        nodes.push_back(alpha);
    }
    else
    {
        TripleAccessor::Ptr access(new TripleAccessor(Triple::OBJECT));
        args[2].bind(access);
    }

    // consistency checks for variables with equal name
    if (args[0].isVariable())
    {
        auto& sAST = args[0].getAST();
        // sub && pred
        if (args[1].isVariable())
        {
            auto& pAST = args[1].getAST();
            if (sAST == pAST)
            {
                TripleConsistency::Ptr check(new TripleConsistency(Triple::SUBJECT, Triple::PREDICATE));
                nodes.push_back(check);
            }
        }
        // sub && obj
        if (args[2].isVariable())
        {
            auto& oAST = args[1].getAST();
            if (sAST == oAST)
            {
                TripleConsistency::Ptr check(new TripleConsistency(Triple::SUBJECT, Triple::OBJECT));
                nodes.push_back(check);
            }
        }
    }
    if (args[1].isVariable())
    {
        auto& pAST = args[1].getAST();
        // pred && obj
        if (args[2].isVariable())
        {
            auto& oAST = args[2].getAST();
            if (pAST == oAST)
            {
                TripleConsistency::Ptr check(new TripleConsistency(Triple::PREDICATE, Triple::OBJECT));
                nodes.push_back(check);
            }
        }
    }
}

} /* rete */
