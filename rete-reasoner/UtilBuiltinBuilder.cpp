#include "UtilBuiltinBuilder.hpp"

namespace rete {
namespace builtin {

PrintNodeBuilder::PrintNodeBuilder()
    : NodeBuilder("print", BuilderType::BUILTIN)
{
}

Builtin::Ptr PrintNodeBuilder::buildBuiltin(ArgumentList& args) const
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
