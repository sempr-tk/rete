#ifndef RETE_INFERREDEVIDENCE_HPP_
#define RETE_INFERREDEVIDENCE_HPP_

#include "Evidence.hpp"
#include "Token.hpp"
#include "Production.hpp"

namespace rete {

class InferredEvidence : public Evidence {
    const Token::Ptr token_;
    const Production::Ptr production_;
public:
    InferredEvidence(const Token::Ptr& token, const Production::Ptr& production);
    bool operator == (const Evidence& other) const override;

    std::string toString() const override;
};

} /* rete */


#endif /* end of include guard: RETE_INFERREDEVIDENCE_HPP_ */
