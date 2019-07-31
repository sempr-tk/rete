#include "Triple.hpp"

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
    return "(" + subject + " " + predicate + " " + object + ")";
}

const std::string Triple::type_("Triple");
const std::string& Triple::type() const
{
    return Triple::type_;
}

bool Triple::operator < (const WME& other) const
{
    if (type() != other.type()) return type() < other.type();

    if (const Triple* t = dynamic_cast<const Triple*>(&other))
    {
        if (subject != t->subject) return subject < t->subject;
        else if (predicate != t->predicate) return predicate < t->predicate;
        else if (object != t->object) return object < t->object;
        else return false;
    }

    throw std::exception(); // although the type() equals the cast failed!
    // TODO: I guess I should rely on the type and just static_cast
}


std::string Triple::fieldName(Triple::Field f)
{
    if (f == Triple::SUBJECT) return "?sub";
    if (f == Triple::PREDICATE) return "?pred";
    if (f == Triple::OBJECT) return "?obj";
    return "";
}

} /* rete */
