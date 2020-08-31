#ifndef RETE_INFERTRIPLE_HPP_
#define RETE_INFERTRIPLE_HPP_

#include "../rete-core/Production.hpp"
#include "../rete-core/Accessors.hpp"

#include "Triple.hpp"

namespace rete {

/**
    A concrete production to infer new triple WMEs.
*/
class InferTriple : public Production {
public:
    using Ptr = std::shared_ptr<Production>;

    /**
        Implements the construction of a single part of the triple -- subject, predicate or object.
        Can be instantiated to return a predefined, constant string, or with an Accessor to get
        an arbitrary value from the matcheed pattern.
    */
    class ConstructHelper {
        bool isPredefined_;
        std::string string_;

        // unique pointer: Prevents modification of the index etc. from the outside.
        std::unique_ptr<AccessorBase> accessor_;

    public:
        ConstructHelper(const std::string& predefined);
        ConstructHelper(const char* predefined);
        ConstructHelper(std::unique_ptr<AccessorBase> accessor);

        ConstructHelper();
        ConstructHelper(ConstructHelper&& other);

        void init(const std::string& predefined);
        void init(const char* predefined);
        void init(std::unique_ptr<AccessorBase> accessor);

        std::string constructFrom(Token::Ptr token) const;

        /* for visualization purposes */
        std::string toString() const;
    };

    /**
        The InferTriple-Production can be configured to infer a triple with static and variable
        parts. The ConstructHelpers can be implicitely created from strings or {int,
        Triple::Field}, with the latter defining where to get the value from: Take the N'th WME in
        the Token, assume it is a Triple and use its specified Triple::Field.
    */
    InferTriple(ConstructHelper&& subject,
                ConstructHelper&& predicate,
                ConstructHelper&& object);

    /**
        For visualization purposes.
    */
    std::string toString() const override;

    /**
        Infer the triple from the token iff the PropagationFlag is ASSERT.
    */
    void execute(Token::Ptr, PropagationFlag, std::vector<WME::Ptr>&) override;

private:
    const ConstructHelper subject_;
    const ConstructHelper predicate_;
    const ConstructHelper object_;
    std::string name_;
};

} /* rete */


#endif /* end of include guard: RETE_INFERTRIPLE_HPP_ */
