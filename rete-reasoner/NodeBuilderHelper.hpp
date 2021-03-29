#ifndef RETE_PARSER_NODE_BUILDER_HELPER_HPP_
#define RETE_PARSER_NODE_BUILDER_HELPER_HPP_

#include <type_traits>
#include "Argument.hpp"
#include "../rete-core/Util.hpp"


namespace rete {
namespace util {

/**
    throws if args has not exactly 'num' arguments
*/
void requireNumberOfArgs(ArgumentList& args, size_t num);

/**
    throws if not min <= args.size() <= max
*/
void requireNumberOfArgs(ArgumentList& args, size_t min, size_t max);

/**
    Pre-Access check used in other helper methods
*/
void argBoundCheck(ArgumentList& args, size_t index);

/**
    throws if args[index] is not an unbound variable.
*/
void requireUnboundVariable(ArgumentList& args, size_t index);



/**
    Implementation to get an interpretation from given an argument that
    is already known to be a constant (not an accessor)
*/
template <class T>
PersistentInterpretation<T>
    interpretationFromConst(ArgumentList& /*args*/, size_t /*index*/)
{
    // I would love to have this as a static assert.
    // But the distinction between interpretationFromConst and
    // interpretationFromAccessor can only be made at runtime.
    throw std::runtime_error("no implementation to get this from a constant");
}

template <>
inline PersistentInterpretation<float>
    interpretationFromConst(ArgumentList& args, size_t index)
{
    auto& arg = args[index];

    if (arg.getAST().isFloat())
    {
        ConstantAccessor<float> acc(arg.getAST().toFloat());
        acc.index() = 0;
        return acc.getInterpretation<float>()->makePersistent();
    }
    else
    {
        throw rete::NodeBuilderException(
                "Argument " + std::to_string(index) +
                " must be a float. Given: " + arg.getAST());
    }
}

template <>
inline PersistentInterpretation<int>
    interpretationFromConst(ArgumentList& args, size_t index)
{
    auto& arg = args[index];

    if (arg.getAST().isInt())
    {
        ConstantAccessor<int> acc(arg.getAST().toInt());
        acc.index() = 0;
        return acc.getInterpretation<int>()->makePersistent();
    }
    else
    {
        throw rete::NodeBuilderException(
                "Argument " + std::to_string(index) +
                " must be an int. Given: " + arg.getAST());
    }
}


template <>
inline PersistentInterpretation<std::string>
    interpretationFromConst(ArgumentList& args, size_t index)
{
    auto& arg = args[index];

    if (arg.getAST().isString() || arg.getAST().isURI())
    {
        ConstantAccessor<std::string> acc(arg.getAST().toString());
        acc.index() = 0;
        return acc.getInterpretation<std::string>()->makePersistent();
    }
    else
    {
        throw rete::NodeBuilderException(
                "Argument " + std::to_string(index) +
                " must be a string. Given: " + arg.getAST());
    }
}

/**
    Implementation to get an interpretation from given an argument that
    is already known to be an accessor (not a const)
*/
template <class T>
PersistentInterpretation<T>
    interpretationFromAccessor(ArgumentList& args, size_t index)
{
    auto& arg = args[index];

    if (arg.isConst())
    {
        throw rete::NodeBuilderException(
                "Argument " + std::to_string(index) +
                " is required to provide an interpretation to '" +
                util::beautified_typename<T>().value +
                "' but is a constant: '" + arg.getAST() + "'");
    }

    auto acc = arg.getAccessor();
    if (!acc)
    {
        throw rete::NodeBuilderException(
                "Argument " + std::to_string(index) +
                " is required to provide an interpretation to '" +
                util::beautified_typename<T>().value +
                "' but is an unbound variable: " + arg.getVariableName());
    }

    auto interp = acc->getInterpretation<T>();
    if (!interp)
    {
        throw rete::NodeBuilderException(
                "Argument " + std::to_string(index) +
                " is required to provide an interpretation to '" +
                util::beautified_typename<T>().value +
                "' (which it does not). Variable: " + arg.getVariableName());
    }

    return interp->makePersistent();

}

/**
    Converts an argument to a PersistentInterpretation of the given type,
    or throws if that is not possible.
*/
template <class T>
PersistentInterpretation<T> requireInterpretation(ArgumentList& args, size_t index)
{
    argBoundCheck(args, index);
    if (args[index].isConst())
        return interpretationFromConst<T>(args, index);
    else
        return interpretationFromAccessor<T>(args, index);
}

/**
    Returns a NumberToNumberConversion based on the given arg
*/
/*
template <class T>
builtin::NumberToNumberConversion<T>
requireToNumberConversion(ArgumentList& args, size_t index)
{
    // TODO
}
*/

/**
    Returns a NumberToStringConversion based on the given arg
*/
/*
builtin::NumberToStringConversion
requireNumberToStringConversion(ArgumentList& args, size_t index); // TODO
*/

}
}
#endif /* include guard: RETE_PARSER_NODE_BUILDER_HELPER_HPP_ */
