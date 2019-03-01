#ifndef RETE_PARSER_ARGUMENT_HPP_
#define RETE_PARSER_ARGUMENT_HPP_

#include "RuleParserAST.hpp"
#include "../rete-core/Accessors.hpp"
#include <vector>

namespace rete {

/**
    Arguments are what are passed to a buildin, or rather, the constructor/builder/... of a builtin.
    The function that constructs the actual node.

    An Argument can be a bound or unbound variable or a constant value. If it is a variable, it has
    a name. In any case the access to the value is granted through an Accessor-object. If the
    variable is unbound, the accessor is a nullptr and must be bound by the builtin
    constructor/builder/.... A variable that's left unbound is an error.

    TODO: So far, Accessors are only used to get values from WMEs and Tokens. How to handle constants?
*/
class Argument {
    // meta info. This "Argument" is constructed from a ast::Argument. Takes ownership.
    std::unique_ptr<ast::Argument> parsedArgument_;

    // if the variable is already bound, the accessor refers to the point in the token where to get
    // the value
    Accessor::Ptr accessor_;

    Argument();
public:
    Argument(Argument&& other); // move ctor
    Argument(const Argument& other) = delete;
    /**
        Returns true if the argument is a (bound or unbound) variable
    */
    bool isVariable() const;

    /**
        Returns true if the argument is a constant.
    */
    bool isConst() const;

    /**
        If the Argument is a variable, returns its name. Else an empty string.
    */
    std::string getVariableName() const;

    /**
        Returns the AST node this was created from.
        Usefull to access constants.
    */
    const ast::Argument& getAST() const;

    /**
        Returns the accessor to the value of this argument or a nullptr if hasValue() is false.
    */
    Accessor::Ptr getAccessor() const;

    /**
        If this is an unbound variable it will be bound to the given accessor.
        Throws an exception if hasValue() is true.
    */
    void bind(Accessor::Ptr);

    /**
        Create an Argument from the AST node and a set of already bound variables. Takes ownership
        of the AST node.
    */
    static Argument createFromAST(std::unique_ptr<ast::Argument> ast, const std::map<std::string, Accessor::Ptr>& bindings);
};


typedef std::vector<Argument> ArgumentList;

} /* rete */


#endif /* end of include guard: RETE_PARSER_ARGUMENT_LIST_HPP_ */
