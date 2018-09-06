#ifndef RETE_EVIDENCE_HPP_
#define RETE_EVIDENCE_HPP_

#include <memory>

namespace rete {

/**
    If a WME in the rete network can occur due to multiple sources (which I call "Evidence" here), e.g. asserted from the outside system, or inferred by different combinations of tokens and productions, it is not always clear when it should be removed.
    This class provides the base for any evidence for a WME. The only requirement is that it must be comparable to any other Evidence, a check for equivalence.
*/
class Evidence {
public:
    using Ptr = std::shared_ptr<Evidence>;
    virtual bool operator == (const Evidence& other) const = 0;
    virtual ~Evidence() = default;

    virtual std::string toString() const = 0;
};

} /* rete */


#endif /* end of include guard: RETE_EVIDENCE_HPP_ */
