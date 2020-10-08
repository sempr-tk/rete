#include "Skolem.hpp"

namespace rete {

Skolem::Skolem(const std::string& id)
    : identifier(id)
{
}

std::string Skolem::toString() const
{
    return "(Skolem: " + identifier + ")";
}

const std::string Skolem::type_("Skolem");
const std::string& Skolem::type() const
{
    return Skolem::type_;
}

bool Skolem::operator < (const WME& other) const
{
    if (type() != other.type()) return type() < other.type();

    if (const Skolem* s = dynamic_cast<const Skolem*>(&other))
    {
        return identifier < s->identifier;
    }
    else
    {
        // cast failed although type() is equal!
        // TODO: just rely on type() and use static_cast?
        throw std::exception();
    }
}



}
