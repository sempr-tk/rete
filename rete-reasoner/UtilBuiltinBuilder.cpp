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

} /* builtin */
} /* rete */
