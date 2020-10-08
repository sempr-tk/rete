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
template <template<class> class BuiltinTemplate>
class MathBulkBuiltinBuilder : public NodeBuilder {
    const std::string name_;

    template <class NumberType>
    void setup(Builtin::Ptr& builtin, AccessorBase::Ptr& accessor,
               PersistentInterpretation<TokenGroup::Ptr> interp) const
    {
        builtin = std::make_shared<BuiltinTemplate<NumberType>>(std::move(interp));
        accessor = std::make_shared<typename TupleWME<NumberType>::template Accessor<0>>();
    }

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

        std::shared_ptr<AccessorBase> resultAccessor;
        Builtin::Ptr builtin;

        // check which is the preferred interpretation of the childAccessor.
        auto preferredInterp =
            interp.interpretation->childAccessor()
                ->getPreferredInterpretation<int, long, size_t, float, double>();

        // based on the result we choose which NumberType to use for the
        // MathBulkBuiltins.
        if (preferredInterp->isOneOf<int>())
            setup<int>(builtin, resultAccessor, std::move(interp));
        else if (preferredInterp->isOneOf<long>())
            setup<long>(builtin, resultAccessor, std::move(interp));
        else if (preferredInterp->isOneOf<size_t>())
            setup<size_t>(builtin, resultAccessor, std::move(interp));
        else if (preferredInterp->isOneOf<float>())
            setup<float>(builtin, resultAccessor, std::move(interp));
        else if (preferredInterp->isOneOf<double>())
            setup<double>(builtin, resultAccessor, std::move(interp));

        // bind result variable
        args[0].bind(resultAccessor);

        return builtin;
    }
};

}
}


#endif /* include guard: RETE_MATH_BULK_BUILTIN_BUILDER_HPP_ */
