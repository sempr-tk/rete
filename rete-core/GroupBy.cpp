#include <algorithm>

#include "GroupBy.hpp"

namespace rete {

void GroupBy::addCriteria(std::unique_ptr<AccessorBase>&& acc)
{
    accessors_.push_back(std::move(acc));
}

std::string GroupBy::getDOTAttr() const
{
    std::string str = "[label=\"GroupBy\\n";
    for (auto& acc : accessors_)
    {
        str += util::dotEscape(acc->toString()) + "\\n";
    }

    str += "\"]";
    return str;
}


bool GroupBy::operator==(const BetaNode& other) const
{
    auto o = dynamic_cast<const GroupBy*>(&other);
    if (o)
    {
        return std::is_permutation(
                accessors_.begin(), accessors_.end(),
                o->accessors_.begin(), o->accessors_.end(),
                [](const std::unique_ptr<AccessorBase>& l,
                   const std::unique_ptr<AccessorBase>& r)
                {
                    return *l == *r;
                });
    }
    else
    {
        return false;
    }
}

void GroupBy::rightActivate(WME::Ptr, PropagationFlag)
{
    throw std::exception();
}


void GroupBy::leftActivate(Token::Ptr token, PropagationFlag flag)
{
    auto bmem = bmem_.lock();
    if (!bmem) throw std::exception();

    if (flag == PropagationFlag::ASSERT)
    {
        // new match:
        // - find the group it belongs to, based on internal values
        // - if none found
        //      - create a new group
        //      - publish ASSERT for new group
        // - else
        //      - add to group
        //      - publish UPDATE for group
        auto group = findMatchingGroup(token);
        if (!group)
        {
            group = std::make_shared<TokenGroup>();
            groups_.insert(group);
            addToGroup(token, group);
            bmem->leftActivate(nullptr, group, PropagationFlag::ASSERT);
        }
        else
        {
            addToGroup(token, group);
            bmem->leftActivate(nullptr, group, PropagationFlag::UPDATE);
        }
    }
    else if (flag == PropagationFlag::RETRACT)
    {
        // remove match
        // - find the group it is in, based on ptr not on values
        //   (as a change in the values might be the reason it is retracted)
        // - remove it from the group
        // - if the group is empty
        //      - remove the group
        //      - publish RETRACT for group
        // - else
        //      - publish UPDATE for group
        auto group = findTokenGroupContainingToken(token);
        removeFromGroup(token, group);
        if (group->token_.empty())
        {
            groups_.erase(group);
            bmem->leftActivate(nullptr, group, PropagationFlag::RETRACT);
        }
        else
        {
            bmem->leftActivate(nullptr, group, PropagationFlag::UPDATE);
        }
    }
    else if (flag == PropagationFlag::UPDATE)
    {
        // internal values changed
        // - find the group it is in, based on ptr not values
        // - check if it still fits the group (compare values with another entry)
        // - if it still matches
        //      - publish UPDATE for group
        // - else
        //      - remove from group
        //      - publish UPDATE for group
        //      - handle as a new match
        auto group = findTokenGroupContainingToken(token);
        if (fitsInGroup(token, group))
        {
            bmem->leftActivate(nullptr, group, PropagationFlag::UPDATE);
        }
        else
        {
            removeFromGroup(token, group);
            bmem->leftActivate(nullptr, group, PropagationFlag::UPDATE);
            this->leftActivate(token, PropagationFlag::ASSERT);
        }
    }
}


TokenGroup::Ptr GroupBy::findTokenGroupContainingToken(Token::Ptr token) const
{
    auto it = groupOfToken_.find(token);
    if (it != groupOfToken_.end())
        return it->second;
    else
        return nullptr;
}

void GroupBy::addToGroup(Token::Ptr token, TokenGroup::Ptr group)
{
    group->token_.insert(token);
    groupOfToken_[token] = group;
}

void GroupBy::removeFromGroup(Token::Ptr token, TokenGroup::Ptr group)
{
    group->token_.erase(token);
    groupOfToken_.erase(token);
}


bool GroupBy::fitsInGroup(Token::Ptr token, TokenGroup::Ptr group) const
{
    for (auto other : group->token_)
    {
        if (token == other) continue;

        for (auto& acc : accessors_)
        {
            auto interpretation = acc->getCommonInterpretation(*acc).first;
            if (!interpretation->valuesEqual(token, other))
            {
                return false;
            }
        }
        // we only need to check the first entry that is not this token itself
        return true;
    }
    // if there is no other entry in the group, there is no mismatch...
    // this does *not* mean there is no better, already established group!
    return true;
}


TokenGroup::Ptr GroupBy::findMatchingGroup(Token::Ptr token) const
{
    for (auto group : groups_)
    {
        if (fitsInGroup(token, group)) return group;
    }
    return nullptr;
}


}
