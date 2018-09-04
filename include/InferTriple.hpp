#ifndef RETE_INFERTRIPLE_HPP_
#define RETE_INFERTRIPLE_HPP_

#include "Production.hpp"
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
        Can be instantiated to return a predefined, constant string, or with an int and a triple
        field to access part of the matching token.
    */
    class ConstructHelper {
        bool isPredefined_;
        std::string string_;
        int tokenOffset_;
        Triple::Field field_;
    public:
        ConstructHelper(const std::string& predefined);
        ConstructHelper(const char* predefined);
        ConstructHelper(int offset, Triple::Field field);

        ConstructHelper();
        void init(const std::string& predefined);
        void init(const char* predefined);
        void init(int offset, Triple::Field field);

        std::string constructFrom(Token::Ptr token) const;

        /* for visualization purposes */
        std::string getName() const;
    };

    /**
        The InferTriple-Production can be configured to infer a triple with static and variable
        parts. The ConstructHelpers can be implicitely created from strings or {int,
        Triple::Field}, with the latter defining where to get the value from: Take the N'th WME in
        the Token, assume it is a Triple and use its specified Triple::Field.
    */
    InferTriple(const ConstructHelper& subject,
                const ConstructHelper& predicate,
                const ConstructHelper& object);

    /**
        For visualization purposes.
    */
    std::string getName() const override;

    /**
        Infer the triple from the token iff the PropagationFlag is ASSERT.
    */
    void execute(Token::Ptr, PropagationFlag, std::vector<WME::Ptr>&) override;

private:
    const ConstructHelper subject_;
    const ConstructHelper predicate_;
    const ConstructHelper object_;
};

} /* rete */


#endif /* end of include guard: RETE_INFERTRIPLE_HPP_ */
