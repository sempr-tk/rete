#include "../include/Triple.hpp"

namespace rete {

Triple::Triple( const std::string& s,
                const std::string& p,
                const std::string& o)
    : subject(s), predicate(p), object(o)
{
}

const std::string& Triple::getField(Field f) const
{
    if (f == SUBJECT) return subject;
    if (f == PREDICATE) return predicate;
    if (f == OBJECT) return object;
    throw std::exception();
}


std::string Triple::toString() const
{
    return "(" + subject + ", " + predicate + ", " + object + ")";
}

} /* rete */
