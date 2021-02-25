#ifndef RETE_SUM_BUILTIN_BUILDER_HPP_
#define RETE_SUM_BUILTIN_BUILDER_HPP_

#include "MathBuiltinBuilder.hpp"

namespace rete {
namespace builtin {

class SumBuiltinBuilder : public MathBuiltinBuilder {
    /**
        Creates a node using the given number type, and returns the node
        and an accessor to get the result of the computation.

        Consumes the given arguments.
    */
    template <class NumberType>
    std::pair<MathBuiltinBaseUntyped::Ptr, AccessorBase::Ptr>
    createNodeAndAccessor() const
    {
        auto node = std::make_shared<
                        MathBuiltin<NumberType, std::plus<NumberType>>
                    >(this->type(), 0);
        auto acc = std::make_shared<
                    typename TupleWME<NumberType>::template Accessor<0>>();

        return std::make_pair(node, acc);
    }

public:
    SumBuiltinBuilder(const std::string& name) : MathBuiltinBuilder(name)
    {
    }

    Builtin::Ptr buildBuiltin(ArgumentList& args) const override
    {
        auto accessors = preProcessArguments(args);

        MathBuiltinBaseUntyped::Ptr node;
        AccessorBase::Ptr acc;

        if (this->isSufficientForOperands<int>(accessors))
        {
            std::tie(node, acc) = createNodeAndAccessor<int>();
        }
        else if (isSufficientForOperands<long>(accessors))
        {
            std::tie(node, acc) = createNodeAndAccessor<long>();
        }
        else if (isSufficientForOperands<size_t>(accessors))
        {
            std::tie(node, acc) = createNodeAndAccessor<size_t>();
        }
        else if (isSufficientForOperands<float>(accessors))
        {
            std::tie(node, acc) = createNodeAndAccessor<float>();
        }
        else // double
        {
            std::tie(node, acc) = createNodeAndAccessor<double>();
        }

        size_t i = 0;
        for (auto& arg : accessors)
        {
            try {
                node->addOperand(std::move(arg));
            } catch (std::exception& e) {
                throw rete::NodeBuilderException(
                        e.what() +
                        std::string(": ") +
                        args[i+1].getAST().toString());
            }

            i++;
        }



        args[0].bind(acc);
        return node;
    }
};

}}


#endif /* include guard: RETE_SUM_BUILTIN_BUILDER_HPP_ */
