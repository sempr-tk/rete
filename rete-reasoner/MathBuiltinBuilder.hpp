#ifndef RETE_MATH_BUILTIN_BUILDER_HPP_
#define RETE_MATH_BUILTIN_BUILDER_HPP_

#include "NodeBuilder.hpp"
#include "Exceptions.hpp"
#include "../rete-core/TupleWME.hpp"
#include "../rete-core/builtins/Math.hpp"

namespace rete {
namespace builtin {

/**
    The templated MathBuiltinBuilder assumes that the given Builtin-type T returns exactly one
    value in a TupleWMEAccessor<float>, that the first argument given must be an unbound variable
    for the result, and that all further arguments are either a accessor or a float.
*/
template <class T>
class MathBuiltinBuilder : public NodeBuilder {
public:
    MathBuiltinBuilder() : NodeBuilder("", BuilderType::BUILTIN)
    {
    }

    std::string type() const override { return T().name(); }

    Builtin::Ptr buildBuiltin(ArgumentList& args) const override
    {
        MathBuiltin::Ptr builtin(new T());

        if (args.size() < 1)
            throw NodeBuilderException("MathBuiltins need at least an output variable."); // at least an output var would be nice...
        if (!args[0].isVariable() || args[0].getAccessor())
            throw NodeBuilderException("MathBuiltins need the first argument to be an unbound variable (to bind the result to!)."); // and it must be unbound

        // TODO: tighter coupling between Builtin and Accessor for the return value?
        AccessorBase::Ptr resultAccessor(new TupleWME<float>::Accessor<0>());
        args[0].bind(resultAccessor);

        for (size_t i = 1; i < args.size(); i++)
        {
            auto& arg = args[i];
            if (arg.isVariable())
            {
                if (!arg.getAccessor())
                    throw NodeBuilderException("MathBuiltinBuilder: Unbound variables in operands are not supported. (Variable: " + arg.getVariableName() + ")");
                std::unique_ptr<AccessorBase> access(arg.getAccessor()->clone());
                builtin->addOperand({std::move(access)});
            }
            else
            {
                auto& ast = arg.getAST();
                if (!ast.isNumber())
                    throw NodeBuilderException("The given constant is not a number: "+  ast); // not a number constant given to math builtin
                builtin->addOperand(ast.toFloat());
            }
        }

        return builtin;
    }
};

} /* builtin */
} /* rete */


#endif /* end of include guard: RETE_MATH_BUILTIN_BUILDER_HPP_ */
