#ifndef RETE_MATH_BUILTIN_BUILDER_HPP_
#define RETE_MATH_BUILTIN_BUILDER_HPP_

#include "NodeBuilder.hpp"
#include "Exceptions.hpp"
#include "../rete-core/TupleWME.hpp"
#include "../rete-core/builtins/Math.hpp"

namespace rete {
namespace builtin {

/**
    The MathBuiltinBuilder is just a helper base for the specific
    implementations that checks if the general setup of arguments if valid
    and converts them to a vector of unique pointers to AccessorBase.

    The first argument must be an unbound variable for the result, and all
    further arguments of some kind of number type.
*/
class MathBuiltinBuilder : public NodeBuilder {
protected:
    std::vector<std::unique_ptr<AccessorBase>>
    preProcessArguments(ArgumentList& args) const
    {
        // at least an output var would be nice...
        if (args.size() < 1)
            throw NodeBuilderException(
                    "MathBuiltins need at least an output variable.");
        if (!args[0].isVariable() || args[0].getAccessor())
            throw NodeBuilderException(
                    "MathBuiltins need the first argument to be an unbound "
                    "variable (to bind the result to!).");

        std::vector<std::unique_ptr<AccessorBase>> operands;
        for (size_t i = 1; i < args.size(); i++)
        {
            auto& arg = args[i];
            if (arg.isVariable())
            {
                if (!arg.getAccessor())
                {
                    throw NodeBuilderException(
                            "MathBuiltinBuilder: Unbound variables in operands "
                            "are not supported. "
                            "(Variable: " + arg.getVariableName() + ")");
                }
                else
                {
                    std::unique_ptr<AccessorBase> access(arg.getAccessor()->clone());
                    operands.push_back(std::move(access));
                }
            }
            else
            {
                auto& ast = arg.getAST();
                if (!ast.isNumber())
                {
                    throw NodeBuilderException(
                            "The given constant is not a number: "+  ast);
                }
                else
                {
                    // TODO: Extend parser to differ between floats and ints!
                    auto access =
                        std::make_unique<ConstantAccessor<float>>(ast.toFloat());
                    access->index() = 0;
                    operands.push_back(std::move(access));
                }
            }
        }

        return operands;
    }


    template <class NumberType>
    bool isSufficientForOperands(
            const std::vector<std::unique_ptr<AccessorBase>>& operands) const
    {
        for (auto& op : operands)
        {
            if (!op->getInterpretation<NumberType>()) return false;
        }

        return true;
    }

    template <class NumberType>
    std::vector<NumberToNumberConversion<NumberType>>
    toConversions(std::vector<std::unique_ptr<AccessorBase>> operands) const
    {
        std::vector<NumberToNumberConversion<NumberType>> conversions;
        for (auto&& op : operands)
        {
            NumberToNumberConversion<NumberType> c(std::move(op));
            conversions.push_back(std::move(c));
        }

        return conversions;
    }

public:
    MathBuiltinBuilder(const std::string& name) : NodeBuilder(name, BuilderType::BUILTIN)
    {
    }
};



} /* builtin */
} /* rete */


#endif /* end of include guard: RETE_MATH_BUILTIN_BUILDER_HPP_ */
