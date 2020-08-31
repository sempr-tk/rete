#ifndef RETE_BUILTIN_MATH_HPP
#define RETE_BUILTIN_MATH_HPP

#include "../Builtin.hpp"
#include "../Accessors.hpp"


#include <vector>
#include <limits>
#include <exception>

namespace rete {
namespace builtin {

/**
    Simple container for constant values or accessor objects.
    Only holds Accessors or floats.
*/
struct NumericValue {
    PersistentInterpretation<float> variable_;
    float constant_;

    NumericValue(float constant)
        : variable_(), constant_(constant)
    {
    }

    NumericValue(PersistentInterpretation<float> var)
        : variable_(std::move(var)), constant_(0)
    {
    }

    NumericValue(NumericValue&& other)
        : variable_(std::move(other.variable_)), constant_(other.constant_)
    {
    }

    bool operator == (const NumericValue& other) const
    {
        if (variable_.accessor && other.variable_.accessor)
        {
            // both are variables -- are they the same?
            return *variable_.accessor == *other.variable_.accessor;
        }
        else if ((variable_.accessor && !other.variable_.accessor) ||
                 (!variable_.accessor && other.variable_.accessor))
        {
            // one is a variable, one a constant
            return false;
        }
        else /*if (!variable_ && !other.variable_)*/
        {
            // both are constants
            return constant_ == other.constant_;
            // TODO: FLOAT EQUALITY! :(
            // Should be okay for constants, as they are written by hand in the
            // rules right? They are not computed by some algorithm, just
            // parsed from string.
        }
    }
};


/**
    Base class for math builtins. Allows adding operands
*/
class MathBuiltin : public Builtin {
protected:
    std::vector<NumericValue> operands_;
    size_t maxOperands_;

    /**
        Constructs the base for math plugins. The name is used for visualization and parsing rules,
        maxOperands specifies the maximum number of operands.
    */
    MathBuiltin(const std::string& name, size_t maxOperands = std::numeric_limits<size_t>::max());

    /**
        Returns a format string for dot visualization
    */
    std::string getDOTAttr() const override;
public:
    using Ptr = std::shared_ptr<MathBuiltin>;
    /**
        Add a constant to the operand list
        \throws if the maximum number of operands is exceeded
    */
    void addOperand(float val);

    /**
        Add a variable to the operand list
        \throws if the accessor doesn't implement a ValueAccessor<float> or the
                maximum number of operands is exceeded
    */
    void addOperand(std::unique_ptr<AccessorBase> var);

    /**
        Equality operator: checks if the names of the operators and all the operands match.
    */
    bool operator == (const BetaNode& other) const override;
};

/**
    Sums all values given through the accessors and stores the result in a new TupleWME
*/
class Sum : public MathBuiltin {
public:
    Sum();
    WME::Ptr process(Token::Ptr) override;
};

/**
    Multiplies all given values
*/
class Mul : public MathBuiltin {
public:
    Mul();
    WME::Ptr process(Token::Ptr) override;
};

/**
    Devides the first operand by the second
*/
class Div : public MathBuiltin {
public:
    Div();
    WME::Ptr process(Token::Ptr) override;
};

} /* builtin */
} /* rete */

#endif /* end of include guard: RETE_BUILTIN_MATH_HPP */
