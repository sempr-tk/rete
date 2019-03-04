#ifndef RETE_EVIDENCE_HPP_
#define RETE_EVIDENCE_HPP_

#include <memory>

namespace rete {

/**
    If a WME in the rete network can occur due to multiple sources (which I call "Evidence" here),
    e.g. asserted from the outside system, or inferred by different combinations of tokens and
    productions, it is not always clear when it should be removed.
    This class provides the base for any evidence for a WME. The only requirement is that it must
    be comparable to any other Evidence, a check for equivalence.

    Note: In order to allow a consistent ordering between evidences of different types, we need a
    way to order the types themselves -- so that all instances of ClassA are always "less than" all
    instances of ClassB, etc, while the ordering of instances of the same class can (and must) be
    implemented by the class itself. This way to order the classes is done through the type-id
    integer. Make sure to use a unique one for all evidence types. (So far I think we only need the
    two which are already implemented...)
*/
class Evidence {
    int typeId_; // used to offer a comparison between objects of different sub-classes.
protected:
    Evidence(int type) : typeId_(type) {}
public:
    using Ptr = std::shared_ptr<Evidence>;
    virtual bool operator == (const Evidence& other) const = 0;
    virtual bool operator < (const Evidence& other) const = 0;
    virtual ~Evidence() = default;

    virtual std::string toString() const = 0;

    /**
        Returns an ID referring to the type of evidence.
        (Used to avoid dynamic_casts and imlement an ordering between different classes)
    */
    int type() const { return typeId_; }
};

} /* rete */


#endif /* end of include guard: RETE_EVIDENCE_HPP_ */
