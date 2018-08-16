#include "../include/Production.hpp"


namespace rete {

Production::Production(int priority)
    : priority_(priority)
{
}

int Production::getPriority() const
{
    return priority_;
}


} /* rete */
