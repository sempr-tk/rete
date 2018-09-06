#ifndef RETE_ASSERTEDEVIDENCE_HPP_
#define RETE_ASSERTEDEVIDENCE_HPP_


#include <string>
#include "Evidence.hpp"

namespace rete {

class AssertedEvidence : public Evidence {
    const std::string source_;
public:
    AssertedEvidence(const std::string&);
    bool operator == (const Evidence& other) const override;

    std::string toString() const override;
};

} /* rete */


#endif /* end of include guard: RETE_ASSERTEDEVIDENCE_HPP_ */
