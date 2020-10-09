#ifndef RETE_MAKESKOLEM_BUILDER_HPP_
#define RETE_MAKESKOLEM_BUILDER_HPP_

#include "NodeBuilder.hpp"

namespace rete {

class MakeSkolemBuilder : public NodeBuilder {
public:
    MakeSkolemBuilder();

    Builtin::Ptr buildBuiltin(ArgumentList& args) const override;
};

}

#endif /* include guard: RETE_MAKESKOLEM_BUILDER_HPP_ */
