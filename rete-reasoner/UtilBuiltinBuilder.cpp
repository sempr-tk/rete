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
                throw NodeBuilderException("unbound variable (" + arg.getVariableName() + ") in print");
            if (!acc->canAs<StringAccessor>())
                throw NodeBuilderException(
                        arg.getVariableName() +
                        "cannot be converted to String. The accessor " +
                        acc->toString() + " does not implement 'StringAccessor'");

            std::unique_ptr<StringAccessor> clone(dynamic_cast<StringAccessor*>(acc->clone()));
            builtin->add(std::move(clone));
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
            if (!acc) throw NodeBuilderException("unbound variable in print effect");
            if (!acc->canAs<StringAccessor>())
            {
                throw NodeBuilderException(
                        arg.getVariableName() +
                        " cannot be converted to String");
            }

            std::unique_ptr<StringAccessor> clone(dynamic_cast<StringAccessor*>(acc->clone()));
            print->add(std::move(clone));
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
