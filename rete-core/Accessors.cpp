#include <algorithm>
#include "Accessors.hpp"

namespace rete {

AccessorBase::AccessorBase(int index)
    : index_(index)
{
}

AccessorBase::~AccessorBase()
{
    for (auto i : interpretations_)
    {
        delete i.second;
    }
}

int& AccessorBase::index()
{
    return index_;
}

int const& AccessorBase::index() const
{
    return index_;
}

std::pair<InterpretationBase*, InterpretationBase*>
    AccessorBase::getCommonInterpretation(const AccessorBase& other) const
{
    for (auto ti : interpretations_)
    {
        for (auto oi : other.interpretations_)
        {
            if (ti.first == oi.first)
            {
                return std::make_pair(ti.second, oi.second);
            }
        }
    }

    return std::make_pair(nullptr, nullptr);
}

std::vector<std::pair<InterpretationBase*, InterpretationBase*>>
    AccessorBase::getCommonInterpretations(const AccessorBase& other) const
{
    std::vector<std::pair<InterpretationBase*, InterpretationBase*>> common;

    for (auto ti : interpretations_)
    {
        for (auto oi : other.interpretations_)
        {
            if (ti.first == oi.first)
            {
                common.push_back(std::make_pair(ti.second, oi.second));
            }
        }
    }

    return common;
}

std::string AccessorBase::toString() const
{
    return "Accessor(" + std::to_string(index_) + ")";
}

} /* rete */
