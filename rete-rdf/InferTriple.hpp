#ifndef RETE_INFERTRIPLE_HPP_
#define RETE_INFERTRIPLE_HPP_

#include "../rete-core/Production.hpp"
#include "../rete-core/Accessors.hpp"

#include "Triple.hpp"
#include "TriplePart.hpp"

namespace rete {

/**
    Performs conversion of
        std::string, float, double, int, long
    to
        TriplePart
    and is used to simplify the implementation of InferTriple.
*/
class ToTriplePartConversion
    : public AccessorConversion<TriplePart,
                                std::string, float, double, int, long> {
public:
    ToTriplePartConversion(std::unique_ptr<AccessorBase>&& acc);

    void convert(const std::string& src, TriplePart& dest) const override;
    void convert(const float& src, TriplePart& dest) const override;
    void convert(const double& src, TriplePart& dest) const override;
    void convert(const int& src, TriplePart& dest) const override;
    void convert(const long& src, TriplePart& dest) const override;
};


/**
    A concrete production to infer new triple WMEs.
*/
class InferTriple : public Production {
public:
    using Ptr = std::shared_ptr<Production>;

    /**
        The InferTriple-Production takes 3 accessors for the (s p o) parts
    */
    InferTriple(ToTriplePartConversion&& subject,
                ToTriplePartConversion&& predicate,
                ToTriplePartConversion&& object);

    /**
        For visualization purposes.
    */
    std::string toString() const override;

    /**
        Infer the triple from the token iff the PropagationFlag is ASSERT.
    */
    void execute(Token::Ptr, PropagationFlag, std::vector<WME::Ptr>&) override;

private:
    // keep alive
    ToTriplePartConversion subject_, predicate_, object_;

};

} /* rete */


#endif /* end of include guard: RETE_INFERTRIPLE_HPP_ */
