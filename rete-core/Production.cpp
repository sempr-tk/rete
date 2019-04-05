#include "Production.hpp"


namespace rete {

Production::Production(int priority, const std::string& name)
    : priority_(priority), name_(name)
{
}

int Production::getPriority() const
{
    return priority_;
}

std::string Production::getName() const
{
    return name_;
}

void Production::setName(const std::string& name)
{
    name_ = name;
}

std::string Production::toString() const
{
    return getName();
}

} /* rete */
