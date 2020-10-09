#ifndef RETE_RDF_SKOLEM_HPP_
#define RETE_RDF_SKOLEM_HPP_

#include <string>
#include "../rete-core/WME.hpp"

namespace rete {

class Skolem : public WME {
    static const std::string type_;
public:
    using Ptr = std::shared_ptr<Skolem>;

    const std::string identifier;

    Skolem(const std::string& id);

    std::string toString() const override;
    const std::string& type() const override;
    bool operator < (const WME& other) const override;
};

}

#endif /* include guard: RETE_RDF_SKOLEM_HPP_ */
