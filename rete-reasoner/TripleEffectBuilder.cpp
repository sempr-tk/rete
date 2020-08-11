#include "TripleEffectBuilder.hpp"
#include "Exceptions.hpp"

namespace rete {

TripleEffectBuilder::TripleEffectBuilder()
    : NodeBuilder("Triple", BuilderType::EFFECT)
{
}

Production::Ptr TripleEffectBuilder::buildEffect(ArgumentList& args) const
{
    // should never happen, the parser knows that triples need 3 arguments
    if (args.size() != 3) throw NodeBuilderException("Wrong number of arguments");

    std::unique_ptr<AccessorBase> acc[3];

    // accessors for subject and predicate must support TriplePart, as strings
    // or numbers make no sense in that position.
    if (args[0].isVariable() &&
        !args[0].getAccessor()->getInterpretation<TriplePart>())
    {
        throw NodeBuilderException("Non-RDF element in subject of triple");
    }

    if (args[1].isVariable() &&
        !args[1].getAccessor()->getInterpretation<TriplePart>())
    {
        throw NodeBuilderException("Non-RDF element in predicate of triple");
    }

    for (int i = 0; i < 3; i++)
    {
        // constants in infer triple arguments are always strings in the correct
        // format, as enforced by the grammar! -> TriplePart, no conversion needed.
        if (args[i].isConst())
        {
            acc[i].reset(new ConstantAccessor<TriplePart>({args[i].getAST()}));
            acc[i]->index() = 0;
        }
        else
            acc[i].reset(args[i].getAccessor()->clone());
    }

    ToTriplePartConversion s(std::move(acc[0])),
                           p(std::move(acc[1])),
                           o(std::move(acc[2]));

    if (!s)
        throw NodeBuilderException("Subject not valid");
    else if (!p)
        throw NodeBuilderException("Predicate not valid");
    else if (!o)
        throw NodeBuilderException("Object not valid");

    InferTriple::Ptr production(
        new InferTriple(std::move(s), std::move(p), std::move(o))
    );

    return production;
}


} /* rete */
