#ifndef RETE_REASONER_EXPLANATION_TO_JSON_VISITOR_HPP_
#define RETE_REASONER_EXPLANATION_TO_JSON_VISITOR_HPP_

#include "ExplanationVisitor.hpp"
#include "WMEToJSONConverter.hpp"

#include <nlohmann/json.hpp>

#include <map>

namespace rete {

/**
 * Visitor that turns the inference graph into a json respresentation as agreed
 * on in our efforts to find a common representation for explanations in
 * general. See: https://git.ni.dfki.de/explain/data-design
 */
class ExplanationToJSONVisitor : public ExplanationVisitor {
    std::map<AssertedEvidence::Ptr, nlohmann::json> processedAssertions_;
    std::map<WME::Ptr, nlohmann::json> processedWMEs_;
    std::map<
        std::pair<Token::Ptr, std::shared_ptr<Annotation>>,
        nlohmann::json
    > processedEffectAnnotations_;
    std::map<InferredEvidence::Ptr, nlohmann::json> processedUngroupedEffects_;

    std::vector<nlohmann::json> groups_; // WMEs grouped and annotated
    std::vector<std::shared_ptr<WMEToJSONConverter>> toJsonConverters_;
    std::map<
        std::pair<std::shared_ptr<GroupByAnnotation>, TokenGroup::Ptr>,
        nlohmann::json
    > processedTokenGroups_;


    size_t lastId_ = 0;

    // helper to create json instances with an assigned id.
    // will not add any other data, only make sure there is a json instance
    // to be filled later.
    size_t getIdOf(InferredEvidence::Ptr);
    size_t getIdOf(AssertedEvidence::Ptr);
    size_t getIdOf(WME::Ptr);

    size_t processTokenGroup(TokenGroup::Ptr, std::shared_ptr<GroupByAnnotation>);

    nlohmann::json wmeToJson(WME::Ptr wme) const;
public:
    ExplanationToJSONVisitor();
    void addToJSONConverter(std::shared_ptr<WMEToJSONConverter> conv);

    bool wantsTokenGroups() const override { return true; }

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