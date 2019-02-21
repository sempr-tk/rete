#include "Math.hpp"
#include "../TupleWME.hpp"
#include "../Util.hpp"

#include <algorithm>

namespace rete {
namespace builtin {

MathBuiltin::MathBuiltin(const std::string& name, size_t maxOperands)
    : Builtin(name), maxOperands_(maxOperands)
{
}

void MathBuiltin::addOperand(float val)
{
    if (maxOperands_ <= operands_.size())
        throw std::exception(); // to many operands given
    operands_.push_back({val});
}

void MathBuiltin::addOperand(std::unique_ptr<Accessor> var)
{
    if (maxOperands_ == operands_.size())
                                throw std::exception(); // to many operands given
    if (!var)                   throw std::exception(); // nullptr?!
    if (!var->canAs<float>())   throw std::exception(); // no conversion to float possible
    operands_.push_back({std::move(var)});
}

std::string MathBuiltin::getDOTAttr() const
{
    std::string s = name() + "(";
    size_t num = 0;
    for (auto& o : operands_)
    {
        if (o.variable_) s += o.variable_->toString();
        else s += std::to_string(o.constant_);
        num++;
        if (num < operands_.size()) s += ", ";
    }
    s += ")";

    return "[label=\"" + util::dotEscape(s) + "\"]";
}

bool MathBuiltin::operator == (const BetaNode& other) const
{
    auto o = dynamic_cast<const MathBuiltin*>(&other);
    if (!o) return false;
    if (o->operands_.size() != this->operands_.size()) return false;

    for (auto& otherOp : o->operands_)
    {
        auto it = std::find(operands_.begin(), operands_.end(), otherOp);
        if (it == operands_.end()) return false;
    }
    return true;
}



// --------- Sum
Sum::Sum()
    : MathBuiltin("sum")
{
}

WME::Ptr Sum::process(Token::Ptr token)
{
    float result = 0.f;
    for (auto& o : operands_)
    {
        if (o.variable_)
            result += o.variable_->value<float>(token);
        else
            result += o.constant_;
    }

    TupleWME<float>::Ptr wme(new TupleWME<float>(result));
    return wme;
}

// --------- Mul
Mul::Mul()
    : MathBuiltin("mul")
{
}

WME::Ptr Mul::process(Token::Ptr token)
{
    float result = 0.f;
    for (auto& o : operands_)
    {
        if (o.variable_)
            result *= o.variable_->value<float>(token);
        else
            result *= o.constant_;
    }

    TupleWME<float>::Ptr wme(new TupleWME<float>(result));
    return wme;
}

// --------- Div
Div::Div()
    : MathBuiltin("div", 2)
{
}

WME::Ptr Div::process(Token::Ptr token)
{
    float numerator, denominator;
    if (operands_[0].variable_) numerator = operands_[0].variable_->value<float>(token);
    else                        numerator = operands_[0].constant_;

    if (operands_[1].variable_) denominator = operands_[1].variable_->value<float>(token);
    else                        denominator = operands_[1].constant_;

    if (denominator == 0.f) throw std::invalid_argument("division by zero!");

    float result = numerator / denominator;

    TupleWME<float>::Ptr wme(new TupleWME<float>(result));
    return wme;
}


} /* builtin */
} /* rete */
