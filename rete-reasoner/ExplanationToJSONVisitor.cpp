#include "ExplanationToJSONVisitor.hpp"
#include "InferenceState.hpp"
#include "../rete-core/Util.hpp"

#include <iostream>

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


void ExplanationToJSONVisitor::visit(WMESupportedBy& support, size_t /* depth */)
{
    getIdOf(support.wme_); // to initialize entry
    nl::json& wmeJson = processedWMEs_[support.wme_];

    std::vector<size_t> supported_by_ids;
    for (auto& ev : support.evidences_)
    {
        size_t evId = getIdOf(ev);
        supported_by_ids.push_back(evId);
    }

    wmeJson["based-on"] = supported_by_ids;
}

void ExplanationToJSONVisitor::visit(WME::Ptr wme, size_t /* depth */)
{
    getIdOf(wme); // to initialize entry
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

void ExplanationToJSONVisitor::visit(AssertedEvidence::Ptr ev, size_t /* depth */)
{
    getIdOf(ev); // to initialize entry
    nl::json& evJson = processedEvidences_[ev];

    evJson["type"] = "reason";
    evJson["value"] = {
        {"description", "Asserted. No explanation, that is just how it is."},
        {"name", ev->toString()}
    };
}

void ExplanationToJSONVisitor::visit(InferredEvidence::Ptr ev, size_t /* depth */)
{
    getIdOf(ev); // to initialize entry
    nl::json& evJson = processedEvidences_[ev];

    evJson["type"] = "reason";
    evJson["value"] = {
        {"description", ev->toString()},
        {"production name", ev->production()->getName()}
    };

    // remember which WMEs have already been dealt with in an annotation group
    std::set<WME::Ptr> wmesInGroups;

    // traverse groups of annotated conditions and create groups of annotated
    // data from it.
    std::vector<size_t> createdGroupIds;
    for (auto& annotation : ev->production()->annotations_)
    {
        nl::json value;
        value["description"] = annotation.annotation_;
        std::cout << "VAR MAPPING SIZE " << annotation.variables_.size() << std::endl;
        for (auto& varMapping : annotation.variables_)
        {
            std::cout << "VAR MAPPING " << varMapping.first << std::endl;
            std::string var = varMapping.first;
            auto acc = varMapping.second;
            if (auto inter = acc->getInterpretation<std::string>())
            {
                std::string varValue;
                inter->getValue(ev->token(), varValue);
                value["variables"][var] = varValue;
            }
            else
            {
                // TODO: This case should be catched early, when parsing the
                // annotation there should be a check if the accessor has a
                // string representation
                value["variables"][var] = "<no string representation available>";
            }
        }

        nl::json group;
        group["type"] = "group";
        group["value"] = value;
        group["id"] = ++lastId_;
        createdGroupIds.push_back(lastId_);

        // traverse token and reference all WMEs that belong to this annotation
        std::vector<size_t> wmeIds;
        Token::Ptr token = ev->token();
        size_t index = 0;
        size_t indexBegin = annotation.tokenIndexBegin_;
        size_t indexEnd = annotation.tokenIndexEnd_;

        std::cout << "getting based-on wme-ids. range " << indexBegin << " - " << indexEnd << std::endl;

        while (token && index < indexEnd)
        {
            if (index >= indexBegin)
            {
                wmesInGroups.insert(token->wme);
                size_t id = getIdOf(token->wme);
                wmeIds.push_back(id);
            }

            index++;
            token = token->parent;
        }

        group["based-on"] = wmeIds;

        // remember annotation group
        this->groups_.push_back(group);
    }

    std::vector<size_t> based_on_wme_ids;
    rete::Token::Ptr token = ev->token();
    while (token)
    {
        // only add "based-on" for wmes that are not in a group
        if (wmesInGroups.find(token->wme) == wmesInGroups.end())
        {
            size_t wmeId = getIdOf(token->wme);
            based_on_wme_ids.push_back(wmeId);
        }
        token = token->parent;
    }

    // add all created group-ids to the based-on attribute
    based_on_wme_ids.insert(
        based_on_wme_ids.end(),
        createdGroupIds.begin(),
        createdGroupIds.end()
    );

    evJson["based-on"] = based_on_wme_ids;
}

nl::json ExplanationToJSONVisitor::json() const
{
    nl::json json = nl::json::array();
    for (auto& entry : processedWMEs_)
    {
        json.push_back(entry.second);
    }

    for (auto& group : groups_)
    {
        json.push_back(group);
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
    groups_.clear();
    lastId_ = 0;
}


}