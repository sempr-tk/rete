#include <set>

#include "../include/Network.hpp"
#include "../include/AlphaMemory.hpp"
#include "../include/AlphaNode.hpp"
#include "../include/BetaMemory.hpp"
#include "../include/BetaNode.hpp"


#include <sstream>
#include <string>

namespace {
    std::string quote(const std::string& str)
    {
        return "\"" + str + "\"";
    }

    std::string ptrToStr(void* ptr)
    {
        std::stringstream ss;
        ss << ptr;
        return ss.str();
    }
}


namespace rete {

void Network::DummyAlpha::activate(WME::Ptr wme)
{
    propagate(wme);
}

bool Network::DummyAlpha::operator == (const AlphaNode& other) const
{
    return (dynamic_cast<const Network::DummyAlpha*>(&other) != nullptr);
}


Network::Network()
    : root_(new Network::DummyAlpha())
{
}

AlphaNode::Ptr Network::getRoot()
{
    return root_;
}


std::string Network::toDot() const
{
    std::string dot = "digraph graphname { \n";

    // 2 phases: alpha net, and beta net
    // phase 1:
    // gather alpha memories as interface to second phase
    std::set<AlphaMemory::Ptr> amems;

    std::set<AlphaNode::Ptr> visitedANodes;
    std::vector<AlphaNode::Ptr> toVisitANodes;

    toVisitANodes.push_back(root_);

    while (!toVisitANodes.empty())
    {
        // get next node to process
        auto last = toVisitANodes.back();
        toVisitANodes.pop_back();

        // alphamemory?
        if (last->hasAlphaMemory())
        {
            auto amem = last->getAlphaMemory();
            amems.insert(amem); // remember for second phase
            // draw edge
            dot += quote("ANode " + ptrToStr(last.get())) + " -> " + quote("AMem " + ptrToStr(amem.get())) + ";\n";
        }

        // get children
        std::vector<AlphaNode::Ptr> children;
        last->getChildren(children);

        // check which ones have been added to the list to visit yet, remember the new ones:
        for (auto c : children)
        {
            if (visitedANodes.find(c) == visitedANodes.end())
            {
                visitedANodes.insert(c);
                toVisitANodes.push_back(c);
            }

            // but for every child, draw last -> child
            dot += quote("ANode " + ptrToStr(last.get())) + " -> " + quote("ANode " + ptrToStr(c.get())) + ";\n";
        }
    }

    // second phase: beta network
    std::set<BetaNode::Ptr> visitedBNodes;
    std::vector<BetaNode::Ptr> toVisitBNodes;

    for (auto amem : amems)
    {
        std::vector<BetaNode::Ptr> children;
        amem->getChildren(children);

        for (auto c : children)
        {
            // draw mem -> child
            dot += quote("AMem " + ptrToStr(amem.get())) + " -> " +
            quote("BNode " + ptrToStr(c.get())) + ";\n";
            
            if (visitedBNodes.find(c) == visitedBNodes.end())
            {
                // add to the nodes to visit
                visitedBNodes.insert(c);
                toVisitBNodes.push_back(c);

            }
        }
    }

    // as long as there are BetaNodes to visit
    while (!toVisitBNodes.empty())
    {
        // get the next node to process
        auto last = toVisitBNodes.back();
        toVisitBNodes.pop_back();

        // draw last -> memory
        dot += quote("BNode " + ptrToStr(last.get())) + " -> " +
                quote("BMem " + ptrToStr(last->getBetaMemory().get())) + ";\n";

        // draw memory -> children
        std::vector<BetaNode::Ptr> children;
        last->getBetaMemory()->getChildren(children);
        for (auto c : children)
        {
            dot += quote("BMem " + ptrToStr(last->getBetaMemory().get())) + " -> " +
                    quote("BNode " + ptrToStr(c.get())) + ";\n";

            if (visitedBNodes.find(c) == visitedBNodes.end())
            {
                visitedBNodes.insert(c);
                toVisitBNodes.push_back(c);
            }
        }
    }

    dot += "}";
    return dot;
}

} /* rete */
