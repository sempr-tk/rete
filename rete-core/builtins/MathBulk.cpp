#include "MathBulk.hpp"
#include "../TupleWME.hpp"
#include <cassert>

namespace rete {
namespace builtin {

MathBulkBuiltin::MathBulkBuiltin(
        const std::string& name,
        PersistentInterpretation<TokenGroup::Ptr> input)
    :
        Builtin(name),
        input_(std::move(input))
{
    childInput_ = input_.interpretation->getChildInterpretation<float>();
    if (!childInput_)
        throw std::invalid_argument(
                "Accessor does not point to float in TokenGroup");
}

std::string MathBulkBuiltin::getDOTAttr() const
{
    std::string s = name() + "(";
    s += input_.accessor->toString();
    s += ")";
    return "[label=\"" + util::dotEscape(s) + "\"]";
}

bool MathBulkBuiltin::operator==(const BetaNode& other) const
{
    auto o = dynamic_cast<const MathBulkBuiltin*>(&other);
    if (!o)
        return false;
    else if (o->name() != this->name())
        return false;
    else
        return *input_.accessor == *o->input_.accessor;
}


SumBulk::SumBulk(PersistentInterpretation<TokenGroup::Ptr> input)
    : MathBulkBuiltin("sum", std::move(input))
{
}

WME::Ptr SumBulk::process(Token::Ptr token)
{
    TokenGroup::Ptr group;
    input_.interpretation->getValue(token, group);
    float sum = 0.f;
    for (auto entry : group->token_)
    {
        float val;
        childInput_->getValue(entry, val);
        sum += val;
    }

    return std::make_shared<TupleWME<float>>(sum);
}


MulBulk::MulBulk(PersistentInterpretation<TokenGroup::Ptr> input)
    : MathBulkBuiltin("mul", std::move(input))
{
}

WME::Ptr MulBulk::process(Token::Ptr token)
{
    TokenGroup::Ptr group;
    input_.interpretation->getValue(token, group);
    float product = 1.f;
    for (auto entry : group->token_)
    {
        float val;
        childInput_->getValue(entry, val);
        product *= val;
    }

    return std::make_shared<TupleWME<float>>(product);
}


}
}
