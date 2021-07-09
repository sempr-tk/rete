#include "ExplanationToJSONVisitor.hpp"
#include "InferenceState.hpp"
#include "../rete-core/Util.hpp"

namespace nl = nlohmann;

namespace rete {

size_t ExplanationToJSONVisitor::getIdOf(Evidence::Ptr ev)
{
   nl::json& evJson = processedEvidences_[ev];
   if (evJson.find("id") == evJson.end())
   {
       evJson["id"] = ++lastId_;
   }

   return evJson["id"];
}

size_t ExplanationToJSONVisitor::getIdOf(WME::Ptr wme)
{
    nl::json& wmeJson = processedWMEs_[wme];
    if (wmeJson.find("id") == wmeJson.end())
    {
        wmeJson["id"] = ++lastId_;
    }

    return wmeJson["id"];
}


void ExplanationToJSONVisitor::visit(WMESupportedBy& support, size_t depth)
{
    size_t wmeId = getIdOf(support.wme_);
    nl::json& wmeJson = processedWMEs_[support.wme_];

    std::vector<size_t> supported_by_ids;
    for (auto& ev : support.evidences_)
    {
        size_t evId = getIdOf(ev);
        supported_by_ids.push_back(evId);
    }

    wmeJson["based-on"] = supported_by_ids;
}

void ExplanationToJSONVisitor::visit(WME::Ptr wme, size_t depth)
{
    size_t wmeId = getIdOf(wme);
    nl::json& wmeJson = processedWMEs_[wme];

    wmeJson["type"] = "data";
    wmeJson["value"] = {
        {"description", "Well, some WME. Not sure what it is. TODO!"},
        {"toString", wme->toString()}
    };
}

void ExplanationToJSONVisitor::visit(Evidence::Ptr, size_t)
{
    // should not ever be used.
    // All evidences are handled as AssertedEvidence or InferredEvidence
}

void ExplanationToJSONVisitor::visit(AssertedEvidence::Ptr ev, size_t depth)
{
    size_t evId = getIdOf(ev);
    nl::json& evJson = processedEvidences_[ev];

    evJson["type"] = "reason";
    evJson["value"] = {
        {"description", "Asserted. No explanation, that is just how it is."},
        {"name", ev->toString()}
    };
}

void ExplanationToJSONVisitor::visit(InferredEvidence::Ptr ev, size_t depth)
{
    size_t evId = getIdOf(ev);
    nl::json& evJson = processedEvidences_[ev];

    evJson["type"] = "reason";
    evJson["value"] = {
        {"description", ev->toString()},
        {"production name", ev->production()->getName()}
    };

    std::vector<size_t> based_on_wme_ids;
    rete::Token::Ptr token = ev->token();
    while (token)
    {
        size_t wmeId = getIdOf(token->wme);
        based_on_wme_ids.push_back(wmeId);
        token = token->parent;
    }

    evJson["based-on"] = based_on_wme_ids;
}

nl::json ExplanationToJSONVisitor::json() const
{
    nl::json json = nl::json::array();
    for (auto& entry : processedWMEs_)
    {
        json.push_back(entry.second);
    }

    for (auto& entry : processedEvidences_)
    {
        json.push_back(entry.second);
    }

    return json;
}

void ExplanationToJSONVisitor::clear()
{
    processedEvidences_.clear();
    processedWMEs_.clear();
    lastId_ = 0;
}


}