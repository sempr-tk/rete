#ifndef RETE_GENERICJOIN_HPP_
#define RETE_GENERICJOIN_HPP_

#include <vector>
#include <utility>
#include <algorithm>

#include "JoinNode.hpp"
#include "Util.hpp"
#include "Accessors.hpp"


#include <iostream>
namespace rete {

/**
    This class implements a generic join-node based on the usage of ValueAccessors.
    It accepts pairs of accessors, where the first is used to access a WME in a token and the second
    to access the WME from an AlphaMemory.

    The actual comparison is done in the ValueAccessors, which is why the GenericJoin does not need
    to know the actual datatypes compared. But make sure that the Accessors can compare their
    accessed values.
*/
class GenericJoin : public JoinNode {
    std::string getDOTAttr() const override
    {
        std::string s = std::string("[label=\"") +
                                (isNegative() ? "negative " : "") + "GenericJoin";
        for (auto check : checks_)
        {
            s = s + "\\n" +
                util::dotEscape(check.leftAccessor->toString()) + " == " +
                util::dotEscape(check.rightAccessor->toString());
        }
        return s +"\"]\n";
    }

    std::string toString() const override
    {
        std::string s = "GenericJoin";
        if (isNegative()) s = "negative " + s;

        for (auto check : checks_)
        {
            s = s + "\n" + check.leftAccessor->toString() + " == " + check.rightAccessor->toString();
        }

        return s;
    }

    typedef std::pair<InterpretationBase*, InterpretationBase*> InterpretationPair;
    /**
        Small helper struct to keep left and right accessors together with
        their common interpretation.
    */
    struct Check {
        AccessorBase::Ptr leftAccessor, rightAccessor;
        InterpretationPair common;
    };
    std::vector<Check> checks_;

public:
    using Ptr = std::shared_ptr<GenericJoin>;
    /**
        Adds an additional check to the join node.
        The left accessor will be applied to a token, while the right accessor is directly used
        with a WME from an alpha memory.

        Make sure that the accessors are configured accordingly, else this will throw.
        Also throws if the two accessors are unable to compare their pointed-at values, i.e.,
        implement different ValueAccessor<T>s.
    */
    void addCheck(AccessorBase::Ptr left, AccessorBase::Ptr right)
    {
        if (left->index() == -1 || right->index() != -1) throw std::exception();

        auto pair = left->getCommonInterpretation(*right);
        if (!std::get<0>(pair) || !std::get<1>(pair))
        {
            // No common interpretation of accessors
            throw std::exception();
        }
        else
        {
            Check c;
            c.leftAccessor = left;
            c.rightAccessor = right;
            c.common = pair;
            checks_.push_back(c);
        }
    }

    bool isValidCombination(Token::Ptr token, WME::Ptr wme) override
    {
        for (auto check : checks_)
        {
            if (!check.common.first->valuesEqual(*check.common.second, token, wme))
            {
                return false;
            }
        }
        return true;
    }

    bool operator == (const BetaNode& other) const override
    {
        if (auto o = dynamic_cast<const GenericJoin*>(&other))
        {
            if (o->isNegative() != this->isNegative()) return false;
            if (o->checks_.size() != this->checks_.size()) return false;
            for (auto check : this->checks_)
            {
                if (std::find_if(o->checks_.begin(), o->checks_.end(),
                        [check](const Check& other) -> bool
                        {
                            return (*other.leftAccessor == *check.leftAccessor) &&
                                   (*other.rightAccessor == *check.rightAccessor);
                        })
                    == o->checks_.end())
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

};

} /* rete */

#endif /* end of include guard: RETE_GENERICJOIN_HPP_ */
