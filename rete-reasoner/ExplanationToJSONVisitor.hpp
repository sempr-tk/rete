#ifndef RETE_REASONER_EXPLANATION_TO_JSON_VISITOR_HPP_
#define RETE_REASONER_EXPLANATION_TO_JSON_VISITOR_HPP_

#include "ExplanationVisitor.hpp"
#include <nlohmann/json.hpp>

#include <map>

namespace rete {

/**
 * Visitor that turns the inference graph into a json respresentation as agreed
 * on in our efforts to find a common representation for explanations in
 * general. See: https://git.ni.dfki.de/explain/data-design
 */
class ExplanationToJSONVisitor : public ExplanationVisitor {
    std::map<Evidence::Ptr, nlohmann::json> processedEvidences_;
    std::map<WME::Ptr, nlohmann::json> processedWMEs_;
    size_t lastId_ = 0;

    // helper to create json instances with an assigned id.
    // will not add any other data, only make sure there is a json instance
    // to be filled later.
    size_t getIdOf(Evidence::Ptr);
    size_t getIdOf(WME::Ptr);
public:
    void visit(WMESupportedBy&, size_t) override;
    void visit(WME::Ptr, size_t) override;
    void visit(Evidence::Ptr, size_t) override;
    void visit(AssertedEvidence::Ptr, size_t) override;
    void visit(InferredEvidence::Ptr, size_t) override;

    nlohmann::json json() const;
    void clear();
};

}



#endif /* RETE_REASONER_EXPLANATION_TO_JSON_VISITOR_HPP_ */