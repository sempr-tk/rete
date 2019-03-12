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
        std::cout << "Build print builtin with argument: " << arg.getAST() << std::endl;
        if (arg.isVariable())
        {
            auto acc = arg.getAccessor();
            if (!acc) throw std::runtime_error("unbound var in print");
            if (!acc->canAs<StringAccessor>()) throw std::runtime_error("must implement StringAccessor");
            std::unique_ptr<StringAccessor> clone(dynamic_cast<StringAccessor*>(acc->clone()));
            builtin->add(std::move(clone));
        }
        else
        {
            std::cout << "its not a variable!" << std::endl;
            builtin->add(std::string(arg.getAST()));
        }
    }

    return builtin;
}

} /* builtin */
} /* rete */
