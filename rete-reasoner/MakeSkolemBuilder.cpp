#include "MakeSkolemBuilder.hpp"
#include "Exceptions.hpp"
#include "../rete-rdf/MakeSkolem.hpp"
#include "../rete-rdf/SkolemAccessor.hpp"
#include "../rete-core/builtins/NumberToStringConversion.hpp"

namespace rete {

MakeSkolemBuilder::MakeSkolemBuilder()
    : NodeBuilder("makeSkolem", NodeBuilder::BuilderType::BUILTIN)
{
}

Builtin::Ptr MakeSkolemBuilder::buildBuiltin(ArgumentList& args) const
{
    if (args.size() < 2) {
        throw NodeBuilderException(
                "Need 2 or more arguments: An unbound variable for the result"
                " and at least one string or number to create the skolem from");
    }
    else if (!args[0].isVariable() || args[0].getAccessor())
    {
        throw NodeBuilderException(
                "First argument must be an unbound variable for the result.");
    }

    auto node = std::make_shared<builtin::MakeSkolem>();

    for (auto& arg : args)
    {
        if (arg.isVariable())
        {
            auto acc = arg.getAccessor();
            if (!acc)
            {
                throw NodeBuilderException(
                        "Variable " + arg.getVariableName() + " is unbound.");
            }
            else
            {
                std::unique_ptr<AccessorBase> clone(acc->clone());
                builtin::NumberToStringConversion conv(std::move(clone));

                if (!conv)
                {
                    throw NodeBuilderException(
                            "Variable " + arg.getVariableName() + " cannot be"
                            " interpreted / converted to a string for the"
                            " skolemization.");
                }

                node->addPart(std::move(conv));
            }
        }
        else /* arg.isConst() */
        {
            // just use the const as is as string.
            auto acc =
                std::make_unique<ConstantAccessor<std::string>>(arg.getAST());
            acc->index() = 0;
            builtin::NumberToStringConversion conv(std::move(acc));

            node->addPart(std::move(conv));
        }
    }


    // the created node adds a "Skolem"-WME to the token, so here we bind the
    // variable to the appropriate SkolemAccessor.

    auto returnAcc = std::make_shared<SkolemAccessor>();
    args[0].bind(returnAcc);

    return node;
}

}
