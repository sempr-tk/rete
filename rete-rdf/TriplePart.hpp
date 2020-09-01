#ifndef RETE_RDF_TRIPLEPART_HPP_
#define RETE_RDF_TRIPLEPART_HPP_

#include <string>
#include "../rete-core/Util.hpp"

namespace rete {

/**
    The TriplePart struct is only a tag to a string that allows us to
    differentiate between actual strings, and strings that represent a part of
    a triple and therefore already contain e.g. quotes around a string value or
    angle brackets around a resource identifier, and can be kept as is if reused
    in a triple.
*/
struct TriplePart {
    std::string value;

    bool operator == (const TriplePart& other) const
    {
        return value == other.value;
    }
};


namespace util {

    template <> std::string to_string(const TriplePart&);
}

}

#endif /* include guard: RETE_RDF_TRIPLEPART_HPP_ */
