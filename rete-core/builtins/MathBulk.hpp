#ifndef RETE_BUILTIN_MATHBULK_HPP_
#define RETE_BUILTIN_MATHBULK_HPP_

#include <stdexcept>

#include "../Builtin.hpp"
#include "../Accessors.hpp"
#include "../TokenGroupAccessor.hpp"
#include "../TupleWME.hpp"

namespace rete {
namespace builtin {

/**
    Base class for mathematical bulk-builtins: Math operations that have exactly
    one output variable, and a single token-group input to operate on.
    E.g. to calculate the sum of all values that fulfill some condition, in
    GROUP BY constructions.
*/
template <class NumberType>
class MathBulkBuiltin : public Builtin {
protected:
    MathBulkBuiltin(
            const std::string& name,
            PersistentInterpretation<TokenGroup::Ptr> input)
        :
        Builtin(name),
        input_(std::move(input))
    {
        childInput_ = input_.interpretation->getChildInterpretation<NumberType>();
        if (!childInput_)
            throw std::invalid_argument(
                    "Accessor does not point to " +
                    util::beautified_typename<NumberType>().value +
                    " in TokenGroup");
    }


    PersistentInterpretation<TokenGroup::Ptr> input_; // access to group
    const Interpretation<NumberType>* childInput_; // single value in token in group
public:
    using Ptr = std::shared_ptr<MathBulkBuiltin>;

    std::string getDOTAttr() const override
    {
        std::string s = name() + "(";
        s += input_.accessor->toString();
        s += ")";
        return "[label=\"" + util::dotEscape(s) + "\"]";
    }

    bool operator == (const BetaNode& other) const override
    {
        auto o = dynamic_cast<const MathBulkBuiltin*>(&other);
        if (!o)
            return false;
        else if (o->name() != this->name())
            return false;
        else
            return *input_.accessor == *o->input_.accessor;
    }
};


template <class NumberType>
class SumBulk : public MathBulkBuiltin<NumberType> {
public:
    SumBulk(PersistentInterpretation<TokenGroup::Ptr> input)
        : MathBulkBuiltin<NumberType>("sum", std::move(input))
    {
    }

    WME::Ptr process(Token::Ptr token) override
    {
        std::string description = "sum( ";
        TokenGroup::Ptr group;
        this->input_.interpretation->getValue(token, group);
        NumberType sum = 0;
        for (auto entry : group->token_)
        {
            NumberType val;
            this->childInput_->getValue(entry, val);
            sum += val;
            description += std::to_string(val) + " ";
        }
        description += ") = " + std::to_string(sum);

        auto wme = std::make_shared<TupleWME<NumberType>>(sum);
        wme->description_ = description;
        return wme;
    }
};

template <class NumberType>
class MulBulk : public MathBulkBuiltin<NumberType> {
public:
    MulBulk(PersistentInterpretation<TokenGroup::Ptr> input)
        : MathBulkBuiltin<NumberType>("mul", std::move(input))
    {
    }

    WME::Ptr process(Token::Ptr token) override
    {
        std::string description = "mul( ";
        TokenGroup::Ptr group;
        this->input_.interpretation->getValue(token, group);
        NumberType product = 1;
        for (auto entry : group->token_)
        {
            NumberType val;
            this->childInput_->getValue(entry, val);
            product *= val;
            description += std::to_string(val) + " ";
        }
        description += ") = " + std::to_string(product);

        auto wme = std::make_shared<TupleWME<NumberType>>(product);
        wme->description_ = description;
        return wme;
    }
};

}
}



#endif /* include guard: RETE_BUILTIN_MATHBULK_HPP_ */
