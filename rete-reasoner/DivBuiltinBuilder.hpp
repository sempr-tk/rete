#ifndef RETE_DIV_BUILTIN_BUILDER_HPP_
#define RETE_DIV_BUILTIN_BUILDER_HPP_

#include "MathBuiltinBuilder.hpp"

namespace rete {
namespace builtin {

class DivBuiltinBuilder : public MathBuiltinBuilder {
    /**
        Creates a node using the given number type, and returns the node
        and an accessor to get the result of the computation.

        Consumes the given arguments.
    */
    template <class NumberType>
    std::pair<Builtin::Ptr, AccessorBase::Ptr>
    createNodeAndAccessor(
            std::vector<std::unique_ptr<AccessorBase>>&& args) const
    {
        auto node = std::make_shared<
                        MathBuiltin<NumberType, std::divides<NumberType>, 2>
                    >(this->type());
        auto acc = std::make_shared<
                    typename TupleWME<NumberType>::template Accessor<0>>();

        for (auto& arg : args)
        {
            node->addOperand(std::move(arg));
        }

        return std::make_pair(node, acc);
    }

public:
    DivBuiltinBuilder(const std::string& name) : MathBuiltinBuilder(name)
    {
    }

    Builtin::Ptr buildBuiltin(ArgumentList& args) const override
    {
        auto accessors = preProcessArguments(args);

        Builtin::Ptr node;
        AccessorBase::Ptr acc;

        if (this->isSufficientForOperands<int>(accessors))
        {
            std::tie(node, acc) = createNodeAndAccessor<int>(std::move(accessors));
        }
        else if (isSufficientForOperands<long>(accessors))
        {
            std::tie(node, acc) = createNodeAndAccessor<long>(std::move(accessors));
        }
        else if (isSufficientForOperands<size_t>(accessors))
        {
            std::tie(node, acc) = createNodeAndAccessor<size_t>(std::move(accessors));
        }
        else if (isSufficientForOperands<float>(accessors))
        {
            std::tie(node, acc) = createNodeAndAccessor<float>(std::move(accessors));
        }
        else // double
        {
            std::tie(node, acc) = createNodeAndAccessor<double>(std::move(accessors));
        }


        args[0].bind(acc);
        return node;
    }
};

}}


#endif /* include guard: RETE_DIV_BUILTIN_BUILDER_HPP_ */
