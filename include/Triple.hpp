#ifndef RETE_TRIPLE_HPP_
#define RETE_TRIPLE_HPP_

#include <string>

#include "WME.hpp"

namespace rete {

class Triple : public WME {
    // used for comparisons
    const std::string concat_;
public:
    using Ptr = std::shared_ptr<Triple>;
    const std::string subject;
    const std::string predicate;
    const std::string object;

    Triple( const std::string& s,
            const std::string& p,
            const std::string& o);

    enum Field {
        SUBJECT,
        PREDICATE,
        OBJECT
    };

    const std::string& getField(Field) const;
    std::string toString() const override;

    bool operator < (const WME& other) const override;

    static std::string fieldName(Triple::Field);
};

} /* rete */

#endif /* end of include guard: RETE_TRIPLE_HPP_ */
