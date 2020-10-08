#ifndef RETE_TOKENGROUP_HPP_
#define RETE_TOKENGROUP_HPP_

#include <set>

#include "WME.hpp"
#include "Token.hpp"

namespace rete {

/**
    The TokenGroup-WME allows grouping multiple sub-matches together for a
    combined processing. This can be useful to e.g. count the number of matches,
    or aggregate all sub-geometries and compute the union/intersection/... of
    all of them.

    TokenGroup-WMEs are the result of a GROUP BY <variables> statement.
*/
class TokenGroup : public WME {
    static const std::string type_;
public:
    using Ptr = std::shared_ptr<TokenGroup>;

    std::set<Token::Ptr> token_;

    std::string toString() const override;
    const std::string& type() const override;
    bool operator < (const WME& other) const override;
};

}


#endif /* include guard: RETE_TOKENGROUP_HPP_ */
