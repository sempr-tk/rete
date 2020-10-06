#include "MakeSkolem.hpp"
#include "../rete-core/Util.hpp"
#include "../rete-core/TupleWME.hpp"
#include "TriplePart.hpp"
#include "Skolem.hpp"

#include <sstream>

namespace rete { namespace builtin {

MakeSkolem::MakeSkolem()
    : Builtin("makeSkolem")
{
}

void MakeSkolem::addPart(NumberToStringConversion part)
{
    parts_.push_back(std::move(part));
}

std::string MakeSkolem::getDOTAttr() const
{
    std::stringstream ss;
    for (auto& part : parts_)
    {
        ss << part.toString() << " ";
    }

    return "[label=\"" + util::dotEscape(ss.str()) + "\"]";
}


bool MakeSkolem::operator == (const BetaNode& other) const
{
    auto o = dynamic_cast<const MakeSkolem*>(&other);
    if (!o)
    {
        return false;
    }
    else if (o->parts_.size() != parts_.size())
    {
        return false;
    }
    else
    {
        for (size_t i = 0; i < parts_.size(); i++)
        {
            if (!parts_[i].hasEqualAccessor(o->parts_[i]))
                return false;
        }

        return true;
    }
}


WME::Ptr MakeSkolem::process(Token::Ptr token)
{
    std::stringstream concat;
    for (auto& part : parts_)
    {
        std::string val;
        part.getValue(token, val);
        concat << val;
    }

    std::string hash = hashing_.digest(concat.str());

    // shortcut: ("_b:" + hash) as blank node label
    // nicer would/could be: use hash to index a global map with nicer labels?
    // e.g. just incrementing _:b0, _:b1, _:b2, ...

    return std::make_shared<Skolem>("_:b" + hash);
}


}}
