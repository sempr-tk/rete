#ifndef RETE_UTIL_BUILTIN_BUILDER_HPP_
#define RETE_UTIL_BUILTIN_BUILDER_HPP_

#include "NodeBuilder.hpp"
#include "../rete-core/TupleWME.hpp"
#include "../rete-core/builtins/Util.hpp"
#include "../rete-core/Accessors.hpp"
#include <algorithm>

namespace rete {
namespace builtin {
/**
    Builder for LT, LE, EQ, NEQ, GE, GT
*/
template <Compare::Mode MODE>
class CompareNodeBuilder : public NodeBuilder {
public:
    CompareNodeBuilder() : NodeBuilder("", BuilderType::BUILTIN)
    {
    }

    std::string type() const override
    {
        std::string name = Compare::ModeName(MODE);
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        return name;
    }

    Builtin::Ptr buildBuiltin(ArgumentList& args) const override
    {
        // Compare::Ptr builtin(new Compare();

        if (args.size() != 2)
        {
            std::string s;
            for (auto& arg : args)
            {
                auto& ast = arg.getAST();
                s += ast + "\n";
            }
            throw std::runtime_error("args size!\n" + s); // need exactly 2 arguments
        }

        // and they must be bound
        if (args[0].isVariable() && !args[0].getAccessor()) throw std::runtime_error("left unbound");
        if (args[1].isVariable() && !args[1].getAccessor()) throw std::runtime_error("right unbound");

        std::unique_ptr<Accessor> left, right;

        if (args[0].isVariable())
            left.reset(args[0].getAccessor()->clone());
        else if (args[0].getAST().isNumber())
        {
            left.reset(new ConstantNumberAccessor<float>(args[0].getAST().toFloat()));
            left->index() = 0;
        }
        else
        {
            left.reset(new ConstantStringAccessor(args[0].getAST()));
            left->index() = 0;
        }

        if (args[1].isVariable())
            right.reset(args[1].getAccessor()->clone());
        else if (args[1].getAST().isNumber())
        {
            right.reset(new ConstantNumberAccessor<float>(args[1].getAST().toFloat()));
            right->index() = 0;
        }
        else
        {
            right.reset(new ConstantStringAccessor(args[1].getAST()));
            right->index() = 0;
        }

        Compare::Ptr builtin(new Compare(MODE, std::move(left), std::move(right)));
        return builtin;
    }
};


class PrintNodeBuilder : public NodeBuilder {
public:
    PrintNodeBuilder();
    Builtin::Ptr buildBuiltin(ArgumentList& args) const override;

};

} /* builtin */
} /* rete */


#endif /* end of include guard: RETE_UTIL_BUILTIN_BUILDER_HPP_ */
