#ifndef RETE_BUILTIN_MATHBULK_HPP_
#define RETE_BUILTIN_MATHBULK_HPP_

#include "../Builtin.hpp"
#include "../Accessors.hpp"
#include "../TokenGroupAccessor.hpp"

namespace rete {
namespace builtin {

/**
    Base class for mathematical bulk-builtins: Math operations that have exactly
    one output variable, and a single token-group input to operate on.
    E.g. to calculate the sum of all values that fulfill some condition, in
    GROUP BY constructions.
*/
class MathBulkBuiltin : public Builtin {
protected:
    MathBulkBuiltin(
            const std::string& name,
            PersistentInterpretation<TokenGroup::Ptr> input);

    PersistentInterpretation<TokenGroup::Ptr> input_; // access to group
    const Interpretation<float>* childInput_; // single value in token in group
public:
    using Ptr = std::shared_ptr<MathBulkBuiltin>;

    std::string getDOTAttr() const override;
    bool operator == (const BetaNode& other) const override;
};


class SumBulk : public MathBulkBuiltin {
public:
    SumBulk(PersistentInterpretation<TokenGroup::Ptr> input);
    WME::Ptr process(Token::Ptr) override;
};

class MulBulk : public MathBulkBuiltin {
public:
    MulBulk(PersistentInterpretation<TokenGroup::Ptr> input);
    WME::Ptr process(Token::Ptr) override;
};

}
}



#endif /* include guard: RETE_BUILTIN_MATHBULK_HPP_ */
