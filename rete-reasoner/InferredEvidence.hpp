#ifndef RETE_INFERREDEVIDENCE_HPP_
#define RETE_INFERREDEVIDENCE_HPP_

#include "Evidence.hpp"
#include "../rete-core/Token.hpp"
#include "../rete-core/Production.hpp"

namespace rete {

class InferredEvidence : public Evidence {
    const Token::Ptr token_;
    const Production::Ptr production_;
public:
    InferredEvidence(const Token::Ptr& token, const Production::Ptr& production);
    bool operator == (const Evidence& other) const override;
    bool operator < (const Evidence& other) const override;

    std::string toString() const override;

    Token::Ptr token() const;

    static const int TypeId = 1; // init allowed because it's const!
};

} /* rete */


#endif /* end of include guard: RETE_INFERREDEVIDENCE_HPP_ */
