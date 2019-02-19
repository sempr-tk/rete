#ifndef TRIPLE_EFFECT_BUILDER_HPP_
#define TRIPLE_EFFECT_BUILDER_HPP_

#include "NodeBuilder.hpp"
#include "../rete-rdf/InferTriple.hpp"

namespace rete {

/**
    Builds the nodes required to infer new Triples as an effect of a rule.
*/
class TripleEffectBuilder : public NodeBuilder {
public:
    TripleEffectBuilder();
    Production::Ptr buildEffect(ArgumentList& args) const;
};

} /* rete */

#endif /* end of include guard: TRIPLE_EFFECT_BUILDER_HPP_ */
