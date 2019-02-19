#include "TripleEffectBuilder.hpp"

namespace rete {

TripleEffectBuilder::TripleEffectBuilder()
    : NodeBuilder("Triple", BuilderType::EFFECT)
{
}

Production::Ptr TripleEffectBuilder::buildEffect(ArgumentList& args) const
{
    if (args.size() != 3) throw std::exception(); // wrong number of arguments
    InferTriple::ConstructHelper sub, pred, obj;

    // if sub/pred/obj is const: just use the string as written in the rule
    if (args[0].isConst()) sub.init(args[0].getAST());
    // else clone the Accessor from the ArgumentList.
    else sub.init(std::unique_ptr<Accessor>(args[0].getAccessor()->clone()));

    if (args[1].isConst()) pred.init(args[1].getAST());
    else pred.init(std::unique_ptr<Accessor>(args[1].getAccessor()->clone()));

    if (args[2].isConst()) obj.init(args[2].getAST());
    else obj.init(std::unique_ptr<Accessor>(args[2].getAccessor()->clone()));

    InferTriple::Ptr production(new InferTriple(std::move(sub), std::move(pred), std::move(obj)));
    return production;
}


} /* rete */
