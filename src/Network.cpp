#include <set>

#include "../include/Network.hpp"
#include "../include/AlphaMemory.hpp"
#include "../include/AlphaNode.hpp"
#include "../include/BetaMemory.hpp"
#include "../include/BetaNode.hpp"
#include "../include/ProductionNode.hpp"


#include <sstream>
#include <string>

namespace {
    std::string drawEdge(rete::Node::Ptr source, rete::Node::Ptr target)
    {
        return source->getDOTId() + " -> " + target->getDOTId() + ";\n";
    }

    std::string drawNode(rete::Node::Ptr node)
    {
        return node->getDOTId() + " " + node->getDOTAttr() + ";\n";
    }
}

namespace rete {

void Network::DummyAlpha::activate(WME::Ptr wme, PropagationFlag flag)
{
    propagate(wme, flag);
}

bool Network::DummyAlpha::operator == (const AlphaNode& other) const
{
    return (dynamic_cast<const Network::DummyAlpha*>(&other) != nullptr);
}

std::string Network::DummyAlpha::getDOTAttr() const
{
    return "[label=Root]";
}


Network::Network()
    : root_(new Network::DummyAlpha()), agenda_(new Agenda())
{
}

Network::~Network()
{
    // tear down the nodes
    root_->tearDown();

    // clear the agenda
    while (!agenda_->empty()) agenda_->pop_front();
}

AlphaNode::Ptr Network::getRoot()
{
    return root_;
}

Agenda::Ptr Network::getAgenda()
{
    return agenda_;
}

std::string Network::toDot() const
{
    std::string dot = "digraph graphname { \n" \
                      "graph [fontname = \"Ubuntu Mono\"]\n" \
                      "node  [fontname = \"Ubuntu Mono\"]\n";

    // 2 phases: alpha net, and beta net
    // phase 1:
    // gather alpha memories as interface to second phase
    std::set<AlphaMemory::Ptr> amems;

    std::set<AlphaNode::Ptr> visitedANodes;
    std::vector<AlphaNode::Ptr> toVisitANodes;

    toVisitANodes.push_back(root_);
    dot += drawNode(root_);

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
            dot += drawNode(amem);
            dot += drawEdge(last, amem);
        }

        // get children
        std::vector<AlphaNode::Ptr> children;
        last->getChildren(children);

        // check which ones have been added to the list to visit yet, remember the new ones:
        for (auto c : children)
        {
            if (visitedANodes.find(c) == visitedANodes.end())
            {
                dot += drawNode(c);
                visitedANodes.insert(c);
                toVisitANodes.push_back(c);
            }

            // but for every child, draw last -> child
            dot += drawEdge(last, c);
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
            dot += drawEdge(amem, c);

            if (visitedBNodes.find(c) == visitedBNodes.end())
            {
                // add to the nodes to visit
                dot += drawNode(c);
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
        dot += drawNode(last->getBetaMemory());
        dot += drawEdge(last, last->getBetaMemory());

        // for the memory, draw all productions
        std::vector<ProductionNode::Ptr> prodNodes;
        last->getBetaMemory()->getProductions(prodNodes);
        for (auto prodNode : prodNodes)
        {
            // assumption: no further checks necessary, every ProductionNode belongs to exactly one
            // BetaMemory
            dot += drawNode(prodNode);
            dot += drawEdge(last->getBetaMemory(), prodNode);
        }

        // draw memory -> children
        std::vector<BetaNode::Ptr> children;
        last->getBetaMemory()->getChildren(children);
        for (auto c : children)
        {
            if (visitedBNodes.find(c) == visitedBNodes.end())
            {
                dot += drawNode(c);
                visitedBNodes.insert(c);
                toVisitBNodes.push_back(c);
            }

            dot += drawEdge(last->getBetaMemory(), c);
        }
    }

    dot += "}";
    return dot;
}

} /* rete */
