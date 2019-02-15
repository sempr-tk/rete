#include "TripleNodeBuilder.hpp"
#include <iostream>

namespace rete {

TripleNodeBuilder::TripleNodeBuilder()
    : NodeBuilder("Triple", BuilderType::ALPHA)
{
}

void TripleNodeBuilder::buildAlpha(ArgumentList& args, std::vector<AlphaNode::Ptr>& nodes) const
{
    std::cout << "TripleBuilder with args:" << std::endl;
    for (auto& arg : args)
    {
        std::cout << "  " << arg.getAST() << std::endl;
    }
    if (args.size() != 3) throw std::exception(); // wrong number of arguments

    // alpha nodes/conditions never get a bound variable.
    assert(args[0].getAccessor().get() == nullptr);
    assert(args[1].getAccessor().get() == nullptr);
    assert(args[2].getAccessor().get() == nullptr);

    // static checks for non variable parts of the triple.
    if (args[0].isConst())
    {
        TripleAlpha::Ptr alpha(new TripleAlpha(Triple::SUBJECT, args[0].getAST()));
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
        TripleAlpha::Ptr alpha(new TripleAlpha(Triple::PREDICATE, args[1].getAST()));
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
        TripleAlpha::Ptr alpha(new TripleAlpha(Triple::OBJECT, args[2].getAST()));
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
