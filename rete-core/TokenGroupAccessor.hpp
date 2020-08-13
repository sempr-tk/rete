#ifndef RETE_TOKENGROUPACCESSOR_HPP_
#define RETE_TOKENGROUPACCESSOR_HPP_

#include "TokenGroup.hpp"
#include "Accessors.hpp"

namespace rete {


/**
    The TokenGroupAccessor does not return any other value than the TokenGroup
    itself. Its main characteristic is the fact that is holds another accessor,
    the original one which can be applied to the tokens in the TokenGroup.
*/
class TokenGroupAccessor : public Accessor<TokenGroup, TokenGroup::Ptr> {
    std::unique_ptr<AccessorBase> child_;

    bool equals(const AccessorBase& other) const override;
public:
    TokenGroupAccessor* clone() const override;
    std::string toString() const override;
    void getValue(TokenGroup::Ptr, TokenGroup::Ptr& value) const override;
};


}

#endif /* include guard: RETE_TOKENGROUPACCESSOR_HPP_ */
