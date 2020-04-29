#ifndef RETE_REASONER_PARSEDRULE_HPP_
#define RETE_REASONER_PARSEDRULE_HPP_

#include <string>
#include <vector>
#include <memory>
#include <rete-core/ProductionNode.hpp>

namespace rete {

/**
    This class contains information about a rule that has been parsed by
    the RuleParser: A numerical id, an optional name, the string from which
    this has been parsed and the list of the production nodes that implement
    the consequences of this rule. This list actually holds the rules alive.
    If they run out of scope the corresponding part of the network will be
    deconstructed.

    On destruction of a ParsedRule it will automatically disconnect from the
    network and retract all information it inferred.
*/
class ParsedRule {
private:
    static size_t nextId()
    {
        static size_t id = 0;
        return ++id;
    }

    const size_t id_; // automatically assigned in ctor
    std::string name_;
    std::string ruleString_;
    std::vector<ProductionNode::Ptr> effectNodes_;

    ParsedRule(); // private ctor, only the RuleParser is supposed
                  // to create this
    friend class RuleParser;
public:
    using Ptr = std::shared_ptr<ParsedRule>;

    ~ParsedRule();

    size_t id() const;
    std::string name() const;
    std::string ruleString() const;
    std::vector<ProductionNode::Ptr> effectNodes() const;

    /**
        Disconnects all contained ProductionNodes from their network and
        retracts all information that was inferred through them.

        This is also called by the destructor of this class. But in case you
        have lost track of all the shared_ptrs to this and want to explicitely
        disconnect the rule at some point, use this method.
    */
    void disconnect();
};


}
#endif /* include guard: RETE_REASONER_PARSEDRULE_HPP_ */
