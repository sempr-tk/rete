#ifndef RETE_BUILTIN_MAKESKOLEM_HPP
#define RETE_BUILTIN_MAKESKOLEM_HPP

#include "rete-core/Builtin.hpp"
#include "rete-core/Accessors.hpp"
#include "rete-core/Hash.hpp"
#include "rete-core/builtins/NumberToStringConversion.hpp"

#include <vector>

namespace rete { namespace builtin {


/**
    The MakeSkolem-node takes an arbitrary number of arguments and creates a
    unique identifier based on its inputs. When given the same arguments again,
    it will create a same identifier.

    Accepts numbers and strings through NumberToStringConversion.

    Note: I'm not sure if "MakeSkolem" is the correct term. I'm just using the
    same terminology as in Apache Jena, where the makeSkolem builtin implements
    the same functionality in a very similar way.
*/
class MakeSkolem : public Builtin {
public:
    using Ptr = std::shared_ptr<MakeSkolem>;

    MakeSkolem();
    void addPart(NumberToStringConversion part);

    WME::Ptr process(Token::Ptr) override;
    bool operator == (const BetaNode& other) const override;
    std::string getDOTAttr() const override;

private:
    std::vector<NumberToStringConversion> parts_;
    util::Hash hashing_;
};

}}


#endif /* include guard: RETE_BUILTIN_MAKESKOLEM_HPP */
