#ifndef RETE_GROUP_BY_HPP_
#define RETE_GROUP_BY_HPP_

#include <set>
#include <map>

#include "BetaNode.hpp"
#include "TokenGroup.hpp"
#include "Accessors.hpp"

namespace rete {

/**
    GroupBy nodes take incoming tokens and group them together based on the
    given criteria. As a result, generated new tokens are shorter - they contain
    exactly one WME, a TokenGroup -- and are generally fewer.
    Please be aware of this during rule parsing, as it breaks the convention of
    nodes always adding one WME to the previous token.
*/
class GroupBy : public BetaNode {
    /**
        The actual groups
    */
    std::set<TokenGroup::Ptr> groups_;

    /**
        When groups are inferred they need to be added to a token.
        This map associates groups with tokens in which they are forwarded.
    */
    std::map<TokenGroup::Ptr, Token::Ptr> tokenOfGroup_;

    /**
        Index in which group the given token was added
    */
    std::map<Token::Ptr, TokenGroup::Ptr> groupOfToken_;

    /**
        Criteria for adding a token to a group.
    */
    std::vector<std::unique_ptr<AccessorBase>> accessors_;


    /**
        Checks if the given token matches the given group, i.e. other tokens
        in the group share the same values for each criteria
    */
    bool fitsInGroup(Token::Ptr, TokenGroup::Ptr) const;

    /**
        Get the token group which already contains the token
    */
    TokenGroup::Ptr findTokenGroupContainingToken(Token::Ptr) const;

    /**
        Add a token to a token group (and update indexing stuff)
    */
    void addToGroup(Token::Ptr, TokenGroup::Ptr);

    /**
        Remove a token from a token group (and update indexing stuff)
    */
    void removeFromGroup(Token::Ptr, TokenGroup::Ptr);

    /**
        Finds a group in which the given token would fit
    */
    TokenGroup::Ptr findMatchingGroup(Token::Ptr) const;



public:
    using Ptr = std::shared_ptr<GroupBy>;
    using WPtr = std::weak_ptr<GroupBy>;

    std::string getDOTAttr() const override;

    /**
        Add an accessor that serves as a criteria for the created groups.
        Groups are created such that every given accessor returns the same
        value for all entries in a group.
    */
    void addCriteria(std::unique_ptr<AccessorBase>&&);

    /**
        Not implemented - throws exception. GroupBy nodes only need a left
        parent.
    */
    void rightActivate(WME::Ptr, PropagationFlag) override;

    /**
        Updates the token groups
    */
    void leftActivate(Token::Ptr, PropagationFlag) override;

    bool operator == (const BetaNode& other) const override;
};


}


#endif /* include guard: RETE_GROUP_BY_HPP_ */
