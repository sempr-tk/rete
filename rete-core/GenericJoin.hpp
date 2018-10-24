#ifndef RETE_GENERICJOIN_HPP_
#define RETE_GENERICJOIN_HPP_

#include <vector>
#include <utility>
#include <algorithm>

#include "JoinNode.hpp"
#include "Util.hpp"
#include "ValueAccessor.hpp"

namespace rete {

/**
    This class implements a generic join-node based on the usage of ValueAccessors.
    It accepts pairs of accessors, where the first is used to access a WME in a token and the second
    to access the WME from an AlphaMemory.

    Of course the GenericJoin-Node needs to know the datatype to work with. The default is
    std::string, but other types may be used.
*/
template <typename T = std::string>
class GenericJoin : public JoinNode {
    std::string getDOTAttr() const override
    {
        std::string s = "[label=\"GenericJoin";
        for (auto check : checks_)
        {
            s = s + "\\n" +
                util::dotEscape(check.first->toString()) + " == " +
                util::dotEscape(check.second->toString());
        }
        return s +"\"]\n";
    }

    typedef std::pair<typename ValueAccessor<T>::Ptr, typename ValueAccessor<T>::Ptr> AccessorPair;
    std::vector<AccessorPair> checks_;
public:
    using Ptr = std::shared_ptr<GenericJoin>;
    /**
        Adds an additional check to the join node.
        The left accessor will be applied to a token, while the right accessor is directly used
        with a WME from an alpha memory.
    */
    void addCheck(typename ValueAccessor<T>::Ptr left, typename ValueAccessor<T>::Ptr right)
    {
        checks_.push_back({left, right});
    }

    bool isValidCombination(Token::Ptr token, WME::Ptr wme) override
    {
        for (auto check : checks_)
        {
            if (check.first->value(token) != check.second->value(wme))
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
            if (o->checks_.size() != this->checks_.size()) return false;
            for (auto check : this->checks_)
            {
                if (std::find(o->checks_.begin(), o->checks_.end(), check) == o->checks_.end())
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
