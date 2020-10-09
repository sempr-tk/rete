#include "UtilBuiltinBuilder.hpp"

namespace rete {
namespace builtin {

PrintNodeBuilder::PrintNodeBuilder()
    : NodeBuilder("print", BuilderType::BUILTIN)
{
}

Builtin::Ptr PrintNodeBuilder::buildBuiltin(ArgumentList& args) const
{
    /*
        Assume it is a group to print as long as all args are token accessors
        with a string interpretation inside.
    */
    bool isGroupPrint = true;
    for (auto& arg : args)
    {
        if (!arg.isVariable() ||
            !arg.getAccessor() ||
            !arg.getAccessor()->getInterpretation<TokenGroup::Ptr>() ||
            !arg.getAccessor()->getInterpretation<TokenGroup::Ptr>()
                              ->getChildInterpretation<std::string>())
        {
            isGroupPrint = false;
        }
    }

    if (isGroupPrint)
        return createGroupPrint(args);
    else
        return createRegularPrint(args);


}

Builtin::Ptr PrintNodeBuilder::createGroupPrint(ArgumentList& args) const
{
    PrintGroup::Ptr builtin(new PrintGroup());
    int index = -1;

    for (auto& arg : args)
    {
        // check on accessors has already been done.
        std::unique_ptr<TokenGroupAccessor> acc(
            static_cast<TokenGroupAccessor*>(arg.getAccessor()->clone())
        );
        if (index == -1)
            index = acc->index();
        else
        {
            if (acc->index() != index)
            {
                throw NodeBuilderException(
                        "Variable " + args[0].getVariableName() + " refers to"
                        " a different TokenGroup than " + arg.getVariableName());
            }
        }

        builtin->add(std::move(acc));
    }

    return builtin;
}

Builtin::Ptr PrintNodeBuilder::createRegularPrint(ArgumentList& args) const
{
    Print::Ptr builtin(new Print());

    for (auto& arg : args)
    {
        if (arg.isVariable())
        {
            auto acc = arg.getAccessor();
            if (!acc)
            {
                throw NodeBuilderException(
                        "unbound variable (" + arg.getVariableName() +
                        ") in print");
            }
            if (!acc->getInterpretation<std::string>())
            {
                throw NodeBuilderException(
                        arg.getVariableName() +
                        "cannot be interpreted as std::string.");
            }
            else
            {
                builtin->add(
                    acc->getInterpretation<std::string>()->makePersistent()
                );
            }
        }
        else
        {
            // need to copy the string, else we'll get dangling references (getAST returns by reference!)
            builtin->add(std::string(arg.getAST()));
        }
    }

    return builtin;
}

PrintEffectNodeBuilder::PrintEffectNodeBuilder()
    : NodeBuilder("print", BuilderType::EFFECT)
{
}


Production::Ptr PrintEffectNodeBuilder::buildEffect(ArgumentList& args) const
{
    auto print = std::make_shared<PrintEffect>();

    for (auto& arg : args)
    {
        if (arg.isVariable())
        {
            auto acc = arg.getAccessor();
            if (!acc)
            {
                throw NodeBuilderException("unbound variable in print effect");
            }
            else if (!acc->getInterpretation<std::string>())
            {
                throw NodeBuilderException(
                        arg.getVariableName() +
                        " cannot be interpreted as std::string");
            }
            else
            {
                print->add(
                    acc->getInterpretation<std::string>()->makePersistent()
                );
            }
        }
        else
        {
            print->add(std::string(arg.getAST()));
        }
    }

    return print;
}


CountEntriesInGroupBuilder::CountEntriesInGroupBuilder()
    : NodeBuilder("count", BuilderType::BUILTIN)
{
}

Builtin::Ptr CountEntriesInGroupBuilder::buildBuiltin(ArgumentList& args) const
{
    if (args.size() != 2)
        throw NodeBuilderException(
                "Need exactly 2 arguments: "
                "A result variable and a group to count");

    if (!args[0].isVariable() || args[0].getAccessor())
        throw NodeBuilderException(
                "First argument must be an unbound variable.");
    if (!args[1].isVariable() || !args[1].getAccessor() ||
        !args[1].getAccessor()->getInterpretation<TokenGroup::Ptr>())
        throw NodeBuilderException(
                "Second argument must refer to a variable in a group.");

    auto pi = args[1].getAccessor()->getInterpretation<TokenGroup::Ptr>()
                                    ->makePersistent();

    auto node = std::make_shared<CountEntriesInGroup>(std::move(pi));
    auto resultAcc = std::make_shared<TupleWME<int>::Accessor<0>>();

    args[0].bind(resultAcc);
    return node;
}


} /* builtin */
} /* rete */
