#ifndef RETE_BUILTIN_MATH_HPP
#define RETE_BUILTIN_MATH_HPP

#include "../Builtin.hpp"
#include "../Accessors.hpp"
#include "NumberToNumberConversion.hpp"
#include "../TupleWME.hpp"

#include <vector>
#include <limits>
#include <exception>
#include <sstream>
#include <algorithm>
#include <numeric>

namespace rete {
namespace builtin {


/**
    Base class for math builtins. Allows adding operands.
    All operands are converted to the given NumberType (static_casts!), and the
    result will thus be of NumberType, too.
*/
template <class NumberType,
          size_t MaxOperands = std::numeric_limits<size_t>::max()>
class MathBuiltinBase : public Builtin {
protected:
    std::vector<NumberToNumberConversion<NumberType>> operands_;

    /**
        Returns a format string for dot visualization
    */
    std::string getDOTAttr() const override
    {
        std::stringstream ss;
        ss << name() << "(";
        for (auto& o : operands_)
        {
            ss << o.toString() << ", ";
        }
        ss << "\b\b" << ")";

        return "[label=\"" + util::dotEscape(ss.str()) + "\"]";
    }

    /**
        Constructs a math builtin. The name is used for
        visualization and parsing rules.
    */
    MathBuiltinBase(const std::string& name)
        : Builtin(name)
    {
    }

public:
    using Ptr = std::shared_ptr<MathBuiltinBase>;

    /**
        Add an operand to the list
        \throws if the accessors value cannot be converted to NumberType, and
                if the maximum number of operands is exceeded
    */
    void addOperand(std::unique_ptr<AccessorBase> var)
    {
        if (MaxOperands == operands_.size())
            throw std::exception();

        if (!var)
            throw std::exception();

        NumberToNumberConversion<NumberType> conv(std::move(var));
        if (!conv)
            throw std::exception();

        operands_.push_back(std::move(conv));
    }

    /**
        Equality operator: checks if the names of the operators and all the operands match.
    */
    bool operator == (const BetaNode& other) const override
    {
        auto o = dynamic_cast<const MathBuiltinBase*>(&other);
        if (!(o &&
              o->operands_.size() == this->operands_.size() &&
              o->name() == this->name()))
        {
            return false;
        }
        else
        {
            for (size_t i = 0; i < this->operands_.size(); i++)
            {
                if (!this->operands_[i].hasEqualAccessor(o->operands_[i]))
                {
                    return false;
                }
            }
            return true;
        }
    }
};


template <class NumberType,
          class Operator,
          size_t MaxOperands = std::numeric_limits<size_t>::max()>
class MathBuiltin
    : public MathBuiltinBase<NumberType, MaxOperands> {
    Operator operator_;
    NumberType identity_;
public:
    MathBuiltin(const std::string& name, NumberType identityElement = 0)
        : MathBuiltinBase<NumberType, MaxOperands>(name),
          identity_(identityElement)
    {
    }

    WME::Ptr process(Token::Ptr token) override
    {
        NumberType result(identity_);
        for (auto& operand : this->operands_)
        {
            NumberType val;
            operand.getValue(token, val);

            result = this->operator_(result, val);
        }

        return std::make_shared<TupleWME<NumberType>>(result);
    }
};


template <class NumberType>
class MathBuiltin<NumberType, std::divides<NumberType>, 2>
    : public MathBuiltinBase<NumberType, 2> {
public:
    MathBuiltin(const std::string& name)
        : MathBuiltinBase<NumberType, 2>(name)
    {
    }

    WME::Ptr process(Token::Ptr token) override
    {
        if (this->operands_.size() != 2) throw std::exception();

        NumberType a, b;
        this->operands_[0].getValue(token, a);
        this->operands_[1].getValue(token, b);

        return std::make_shared<TupleWME<NumberType>>(a/b);
    }
};


} /* builtin */
} /* rete */

#endif /* end of include guard: RETE_BUILTIN_MATH_HPP */
