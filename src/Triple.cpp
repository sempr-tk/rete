#include "../include/Triple.hpp"

namespace rete {

Triple::Triple( const std::string& s,
                const std::string& p,
                const std::string& o)
    : concat_(s + p + o), subject(s), predicate(p), object(o)
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
    return "(" + subject + " " + predicate + " " + object + ")";
}


bool Triple::operator < (const WME& other) const
{
    if (const Triple* t = dynamic_cast<const Triple*>(&other))
    {
        return concat_ < t->concat_;
    }
    return true;
}


std::string Triple::fieldName(Triple::Field f)
{
    if (f == Triple::SUBJECT) return "?sub";
    if (f == Triple::PREDICATE) return "?pred";
    if (f == Triple::OBJECT) return "?obj";
    return "";
}

} /* rete */
