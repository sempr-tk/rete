#include "ParsedRule.hpp"
#include "../rete-core/connect.hpp"

namespace rete {

ParsedRule::ParsedRule()
    : id_(ParsedRule::nextId())
{
}

ParsedRule::~ParsedRule()
{
    disconnect();
}

size_t ParsedRule::id() const
{
    return id_;
}

std::string ParsedRule::name() const
{
    return name_;
}

std::string ParsedRule::ruleString() const
{
    return ruleString_;
}

std::vector<ProductionNode::Ptr> ParsedRule::effectNodes() const
{
    return effectNodes_;
}


void ParsedRule::disconnect()
{
    for (auto production : effectNodes_)
    {
        // this actually retracts all data inferred by the production
        SetParent(nullptr, production);
    }
}


}
