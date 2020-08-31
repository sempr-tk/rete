#include "Argument.hpp"

namespace rete {

Argument::Argument()
{
}

Argument::Argument(Argument&& other)
    : parsedArgument_(std::move(other.parsedArgument_)),
      accessor_(std::move(other.accessor_))
{
}

bool Argument::isVariable() const
{
    return parsedArgument_->isVariable();
}

bool Argument::isConst() const
{
    return !isVariable();
}


std::string Argument::getVariableName() const
{
    // if (!isVariable()) throw std::exception();
    // empty string if not a variable
    if (!isVariable()) return "";
    return *parsedArgument_;
}

const ast::Argument& Argument::getAST() const
{
    return *parsedArgument_;
}

AccessorBase::Ptr Argument::getAccessor() const
{
    return accessor_;
}

void Argument::bind(AccessorBase::Ptr accessor)
{
    if (accessor_) throw std::exception(); // TODO custom exception: variable already bound!
    accessor_ = accessor;
}

Argument Argument::createFromAST(
        std::unique_ptr<ast::Argument> ast,
        const std::map<std::string, AccessorBase::Ptr>& bindings)
{
    Argument arg;

    if (ast->isVariable())
    {
        auto it = bindings.find(*ast);
        if (it != bindings.end())
        {
            // create a copy of the accessor to keep the correct index!
            arg.accessor_ = AccessorBase::Ptr(it->second->clone());
        }
    }
    arg.parsedArgument_ = std::move(ast);

    return arg;
}


} /* rete */
