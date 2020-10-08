#ifndef RETE_MATH_BULK_BUILTIN_BUILDER_HPP_
#define RETE_MATH_BULK_BUILTIN_BUILDER_HPP_

#include "NodeBuilder.hpp"
#include "Exceptions.hpp"
#include "../rete-core/TupleWME.hpp"
#include "../rete-core/builtins/MathBulk.hpp"

namespace rete {
namespace builtin {

/**
    The MathBulkBuiltinBuilder template assumes that the builtins accept exactly
    two arguments: An unbound variable for the result, and an accessor to a
    TokenGroup::Ptr, pointing at floats. Also, it assumes the output is a
    TupleWME<float>.
*/
template <class T>
class MathBulkBuiltinBuilder : public NodeBuilder {
    const std::string name_;
public:
    MathBulkBuiltinBuilder(const std::string& name)
        :
            NodeBuilder("", BuilderType::BUILTIN),
            name_(name)
    {
    }

    std::string type() const override { return name_; }

    Builtin::Ptr buildBuiltin(ArgumentList& args) const override
    {
        if (args.size() != 2)
            throw NodeBuilderException(
                    "Need exactly 2 arguments: An unbound"
                    " variable for the result, and a group of floats.");
        else if (!args[0].isVariable() || args[0].getAccessor())
            throw NodeBuilderException(
                    "The first argument must be an unbound variable"
                    " for the result");
        else if (!args[1].isVariable() || !args[1].getAccessor() ||
                 !args[1].getAccessor()->getInterpretation<TokenGroup::Ptr>() ||
                 !args[1].getAccessor()->getInterpretation<TokenGroup::Ptr>()
                                         ->getChildInterpretation<float>())
            throw NodeBuilderException(
                    "The second argument must point to floats in a token group"
                    " as created by a GROUP BY statement");

        // get the input interpretation
        auto interp = args[1].getAccessor()
                        ->getInterpretation<TokenGroup::Ptr>()
                        ->makePersistent();
        //  create the node
        MathBulkBuiltin::Ptr builtin(new T(std::move(interp)));

        // bind result variable
        auto resultAccessor = std::make_shared<TupleWME<float>::Accessor<0>>();
        args[0].bind(resultAccessor);

        return builtin;
    }
};

}
}


#endif /* include guard: RETE_MATH_BULK_BUILTIN_BUILDER_HPP_ */
