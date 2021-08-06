#ifndef RETE_WME_HPP_
#define RETE_WME_HPP_

#include <memory>

namespace rete {

    class Builtin; // forward declaration of builtin base class.

/**
    The base class for all working memory elements in the rete network.
    These are the facts to be added.

    Also, Builtins can compute values and add them as WMEs to a token.
    TODO: This return value of a Builtin should probably become a class of its own, but for now it should suffice to use WMEs for that. There's one catch: Computed values are reasoner-internal. They are not added to the fact base as asserted/inferred values, but are computations that are only valid inside the execution context of a rule. So we need to mark them as such computations, that they don't need to be checked for evidence in the fact-base when checking which facts still hold. --> isComputed_ flag.
*/
class WME {
    bool isComputed_;
    friend class Builtin; // the Builtin base class sets the isComputed_ value
    friend class JoinNode; // negative joins add empty tuples that need to be marked as computed
    friend class TrueNode; // the TrueNode propagates a single EmptyWME that is not asserted but shall always hold
public:

    /**
     * An optional description of the WME.
     * Introduced to allow checks and computations to annotate their otherwise
     * rather non-descriptive results. E.g., "le(?x 5)" results in "TupleWME<>()",
     * but can annotate it with the description "3 <= 5"
     * (assuming that ?x is bound to 3 in this example).
     *
     * This attribute is NOT considered in equality checks of WMEs.
     */
    std::string description_;

    using Ptr = std::shared_ptr<WME>;
    WME();
    virtual ~WME() {};

    bool isComputed() const;

    /**
        For visualization only
    */
    virtual std::string toString() const = 0;


    /**
        Returns a string that unambiguously identifies the type of wme.
        It shall be used in the operator < to consistently compare WMEs of different types.
    */
    virtual const std::string& type() const = 0;


    /**
        In order to keep unique sets of WMEs and find identical ones, we need a less-than
        comparison for them.
        It is advised to first check if the "type()" identifiers are different, and if they are use
        those for the comparison. Only if they are equal, and therefore the other wme can be casted
        to the type of this, do a by-internal-value-comparison.
    */
    virtual bool operator < (const WME& other) const = 0;

    /**
        Check for equality between two WMEs. First checks the memory addresses, then uses the
        operator < to
            return !(a < b) && !(b < a);

    */
    bool operator == (const WME& other) const;
};

} /* rete */


#endif /* end of include guard: RETE_WME_HPP_ */
