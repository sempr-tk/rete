#include "NodeBuilderHelper.hpp"

namespace rete {
namespace util {

void requireNumberOfArgs(ArgumentList& args, size_t num)
{
    if (args.size() != num)
    {
        throw rete::NodeBuilderException(
                "Invalid number of arguments. Need exactly " + std::to_string(num) +
                " but got " + std::to_string(args.size()) + ".");
    }
}

void requireNumberOfArgs(ArgumentList& args, size_t min, size_t max)
{
    if (args.size() < min || args.size() > max)
    {
        throw rete::NodeBuilderException(
                "Invalid number of arguments. Need " +
                std::to_string(min) + " to " + std::to_string(max) +
                " but got " + std::to_string(args.size()) + ".");
    }
}


void argBoundCheck(ArgumentList& args, size_t index)
{
    if (args.size() <= index)
    {
        throw std::runtime_error("ArgumentList out of bounds access");
    }
}


void requireUnboundVariable(ArgumentList& args, size_t index)
{
    argBoundCheck(args, index);
    if (args[index].isConst() || args[index].getAccessor())
    {
        throw rete::NodeBuilderException(
                "Argument " + std::to_string(index) +
                " must be an unbound variable.");
    }
}

builtin::NumberToStringConversion
requireNumberToStringConversion(ArgumentList& args, size_t index)
{
    argBoundCheck(args, index);
    auto& arg = args[index];

    std::unique_ptr<AccessorBase> accessor;
    if (arg.isConst())
    {
        if (arg.getAST().isInt())
            accessor.reset(new ConstantAccessor<int>(arg.getAST().toInt()));
        else if (arg.getAST().isFloat())
            accessor.reset(new ConstantAccessor<float>(arg.getAST().toFloat()));
        else
            accessor.reset(new ConstantAccessor<std::string>(arg.getAST().toString()));
        accessor->index() = 0;
    }
    else
    {
        if (arg.getAccessor())
        {
            accessor.reset(arg.getAccessor()->clone());
        }
        else
        {
            throw NodeBuilderException(
                    "Variable '" + arg.getVariableName() +
                    "' must not be unbound.");
        }
    }

    builtin::NumberToStringConversion conv(std::move(accessor));
    if (!conv)
    {
        throw NodeBuilderException(
                "Argument '" + arg.getAST() +
                "' cannot be converted to string.");
    }

    return conv;
}
}
}
