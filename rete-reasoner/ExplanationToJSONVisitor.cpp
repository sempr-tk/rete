#include "ExplanationToJSONVisitor.hpp"
#include "InferenceState.hpp"
#include "../rete-core/Util.hpp"

#include <iostream>

namespace nl = nlohmann;

namespace rete {

/**
 * Special treatment for inferred evidences:
 * Instead of creating one explanation-element for every production of a rule,
 * only create one for each annotated-effect-group. But keep in mind that there
 * need to be different instances in case different tokens are used!
 */
size_t ExplanationToJSONVisitor::getIdOf(InferredEvidence::Ptr ev)
{
    if (ev->production()->effectAnnotation_)
    {
        auto key = std::make_pair(ev->token(), ev->production()->effectAnnotation_);
        nl::json& evJson = processedEffectAnnotations_[key];
        if (evJson.find("id") == evJson.end())
        {
            evJson["id"] = ++lastId_;
        }

        return evJson["id"];
    }
    else
    {
        // no annotation -> create one explanation element each
        nl::json& evJson = processedUngroupedEffects_[ev];
        if (evJson.find("id") == evJson.end())
        {
            evJson["id"] = ++lastId_;
        }

        return evJson["id"];
    }
}

size_t ExplanationToJSONVisitor::getIdOf(AssertedEvidence::Ptr ev)
{
   nl::json& evJson = processedAssertions_[ev];
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

ExplanationToJSONVisitor::ExplanationToJSONVisitor()
{
    this->addToJSONConverter(
        std::make_shared<DefaultToJSONConverter>()
    );

    this->addToJSONConverter(
        std::make_shared<TripleToJSONConverter>()
    );
}


void ExplanationToJSONVisitor::addToJSONConverter(std::shared_ptr<WMEToJSONConverter> conv)
{
    this->toJsonConverters_.insert(toJsonConverters_.begin(), conv);
}


nl::json ExplanationToJSONVisitor::wmeToJson(WME::Ptr wme) const
{
    std::string tmp;
    for(auto conv : this->toJsonConverters_)
    {
        if (conv->convert(wme, tmp)) break;
    }

    return nl::json::parse(tmp);
}

void ExplanationToJSONVisitor::visit(WMESupportedBy& support, size_t /* depth */)
{
    if (std::dynamic_pointer_cast<TokenGroup>(support.wme_))
        return;

    getIdOf(support.wme_); // to initialize entry
    nl::json& wmeJson = processedWMEs_[support.wme_];

    std::vector<size_t> supported_by_ids;
    for (auto& ev : support.evidences_)
    {
        auto assertedEv = std::dynamic_pointer_cast<AssertedEvidence>(ev);
        if (assertedEv)
        {
            size_t evId = getIdOf(assertedEv);
            supported_by_ids.push_back(evId);
        }
        else
        {
            auto inferredEv = std::dynamic_pointer_cast<InferredEvidence>(ev);
            size_t evId = getIdOf(inferredEv);
            supported_by_ids.push_back(evId);
        }
    }

    wmeJson["based_on"] = supported_by_ids;
}

void ExplanationToJSONVisitor::visit(WME::Ptr wme, size_t /* depth */)
{
    // skip TokenGroups, they are handled separately when needed
    if (std::dynamic_pointer_cast<TokenGroup>(wme))
        return;

    getIdOf(wme); // to initialize entry
    nl::json& wmeJson = processedWMEs_[wme];

    wmeJson["type"] = "data";
    wmeJson["value"] = wmeToJson(wme);
}

void ExplanationToJSONVisitor::visit(Evidence::Ptr, size_t)
{
    // should not ever be used.
    // All evidences are handled as AssertedEvidence or InferredEvidence
}

void ExplanationToJSONVisitor::visit(AssertedEvidence::Ptr ev, size_t /* depth */)
{
    getIdOf(ev); // to initialize entry
    nl::json& evJson = processedAssertions_[ev];

    evJson["type"] = "reason";
    evJson["value"] = {
        {"description", "Asserted. No explanation, that is just how it is."},
        {"name", ev->toString()}
    };
}



void setupVarMapping(nl::json& json, Annotation& annotation, Token::Ptr token)
{
    std::cout << "VAR MAPPING SIZE " << annotation.variables_.size() << std::endl;
    for (auto& varMapping : annotation.variables_)
    {
        std::cout << "VAR MAPPING " << varMapping.first << std::endl;
        std::string var = varMapping.first;
        auto acc = varMapping.second;
        if (auto inter = acc->getInterpretation<std::string>())
        {
            std::string varValue;
            inter->getValue(token, varValue);
            json["variables"][var] = varValue;
        }
        else
        {
            // TODO: This case should be catched early, when parsing the
            // annotation there should be a check if the accessor has a
            // string representation
            json["variables"][var] = "<no string representation available>";
        }
    }
}


std::vector<WME::Ptr> getReferencedWMEs(const Annotation& annotation, Token::Ptr token)
{
    // traverse token and reference all WMEs that belong to this annotation
    std::vector<WME::Ptr> wmes;
    size_t index = 0;
    size_t indexBegin = annotation.tokenIndexBegin_;
    size_t indexEnd = annotation.tokenIndexEnd_;

    std::cout << "getting based_on wme-ids. range " << indexBegin << " - " << indexEnd << std::endl;

    while (token && index < indexEnd)
    {
        if (index >= indexBegin)
        {
            wmes.push_back(token->wme);
        }
        index++;
        token = token->parent;
    }

    return wmes;
}


void ExplanationToJSONVisitor::visit(InferredEvidence::Ptr ev, size_t /* depth */)
{
    getIdOf(ev); // to initialize entry
    auto getJson = [this](InferredEvidence::Ptr ev) -> nl::json&
    {
        if (ev->production()->effectAnnotation_)
        {
            auto key = std::make_pair(ev->token(), ev->production()->effectAnnotation_);
            return this->processedEffectAnnotations_[key];
        }
        else
        {
            return this->processedUngroupedEffects_[ev];
        }
    };

    nl::json& evJson = getJson(ev);

    evJson["type"] = "reason";
    nl::json value;

    auto effectAnnotation = ev->production()->effectAnnotation_;
    if (effectAnnotation)
    {
        std::string reasonName = ev->production()->getName();
        // dont confuse the user with indices
        auto indexPosition = reasonName.rfind('[');
        if (indexPosition != std::string::npos)
        {
            reasonName = reasonName.substr(0, indexPosition);
        }

        value["name"] = reasonName;
        value["description"] = effectAnnotation->annotation_;
        for (auto& varMapping : effectAnnotation->variables_)
        {
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
                // TODO: This should be catched early, when parsing the rule
                value["variables"][var] = "<no string representation available>";
            }
        }
    }
    else
    {
        std::string reasonName = ev->production()->getName();
        // dont confuse the user with indices
        auto indexPosition = reasonName.rfind('[');
        if (indexPosition != std::string::npos)
        {
            reasonName = reasonName.substr(0, indexPosition);
        }

        value["name"] = reasonName;
        value["description"] = "";
    }

    evJson["value"] = value;


    // remember which WMEs have already been dealt with in an annotation group
    std::set<WME::Ptr> wmesInGroups;

    // traverse groups of annotated conditions and create groups of annotated
    // data from it.
    std::vector<size_t> createdGroupIds;
    for (auto& annotation : ev->production()->conditionAnnotations_)
    {
        nl::json value;
        value["description"] = annotation.annotation_;
        setupVarMapping(value, annotation, ev->token());

        nl::json group;
        group["type"] = "group";
        group["value"] = value;
        group["id"] = ++lastId_;
        createdGroupIds.push_back(lastId_);

        auto referencedWMEs = getReferencedWMEs(annotation, ev->token());
        std::vector<size_t> wmeIds;
        std::for_each(referencedWMEs.begin(), referencedWMEs.end(),
            [&](WME::Ptr wme)
            {
                // check if it is a tokengroup
                if (auto tg = std::dynamic_pointer_cast<TokenGroup>(wme))
                {
                    size_t tgId = this->processTokenGroup(tg, ev->production()->groupByAnnotation_);
                    wmeIds.push_back(tgId);
                }
                else
                {
                    wmeIds.push_back(this->getIdOf(wme));
                }
                wmesInGroups.insert(wme);
            }
        );

        group["based_on"] = wmeIds;

        // remember annotation group
        this->groups_.push_back(group);
    }

    std::vector<size_t> based_on_wme_ids;
    rete::Token::Ptr token = ev->token();
    while (token)
    {
        // only add "based_on" for wmes that are not in a group
        if (wmesInGroups.find(token->wme) == wmesInGroups.end())
        {
            // check if it is a tokengroup
            if (auto tg = std::dynamic_pointer_cast<TokenGroup>(token->wme))
            {
                size_t tgId = this->processTokenGroup(tg, ev->production()->groupByAnnotation_);
                based_on_wme_ids.push_back(tgId);
            }
            else
            {
                size_t wmeId = getIdOf(token->wme);
                based_on_wme_ids.push_back(wmeId);
            }
        }
        token = token->parent;
    }

    // add all created group-ids to the based_on attribute
    based_on_wme_ids.insert(
        based_on_wme_ids.end(),
        createdGroupIds.begin(),
        createdGroupIds.end()
    );

    evJson["based_on"] = based_on_wme_ids;
}


size_t ExplanationToJSONVisitor::processTokenGroup(TokenGroup::Ptr tg, std::shared_ptr<GroupByAnnotation> annotation)
{
    // check if this combination has been processed before
    auto key = std::make_pair(annotation, tg);
    auto jsonIt = processedTokenGroups_.find(key);
    if (jsonIt != processedTokenGroups_.end())
    {
        return jsonIt->second["id"];
    }

    nl::json& tgJSON = processedTokenGroups_[key];
    tgJSON["id"] = ++lastId_;
    tgJSON["type"] = "group";
    tgJSON["value"] = "TokenGroup";

    std::vector<size_t> tokenIDs;
    for (auto& token : tg->token_)
    {
        nl::json tJSON;
        tJSON["id"] = ++lastId_;
        tJSON["type"] = "Group";
        tJSON["value"] = "Token";
        tokenIDs.push_back(tJSON["id"]);

        std::vector<size_t> createdGroupIds;
        std::set<WME::Ptr> wmesInGroups;
        // search for nested token-groups first
        // {
        //     auto t = token;
        //     while (t)
        //     {
        //         auto wme = t->wme;
        //         auto nestedTG = std::dynamic_pointer_cast<TokenGroup>(wme);
        //         if (nestedTG)
        //         {
        //             size_t tgId;
        //             if (annotation)
        //                 tgId = processTokenGroup(nestedTG, annotation->parent_);
        //             else
        //                 tgId = processTokenGroup(nestedTG, nullptr);
        //             createdGroupIds.push_back(tgId);
        //         }

        //         t = t->parent;
        //     }
        // }

        // handle annotated groups of wmes
        if (annotation)
        {
            for (auto& a : annotation->annotations_)
            {
                nl::json group;
                group["type"] = "group";
                group["id"] = ++lastId_;

                nl::json value;
                value["description"] = a.annotation_;
                setupVarMapping(value, a, token);

                group["value"] = value;

                auto refWME = getReferencedWMEs(a, token);
                std::vector<size_t> wmeIds;
                std::for_each(refWME.begin(), refWME.end(),
                    [&](WME::Ptr wme)
                    {
                        if (auto tg = std::dynamic_pointer_cast<TokenGroup>(wme))
                        {
                            size_t tgId = this->processTokenGroup(tg, annotation->parent_);
                            wmeIds.push_back(tgId);
                        }
                        else
                        {
                            wmeIds.push_back(this->getIdOf(wme));
                        }
                        wmesInGroups.insert(wme);
                    }
                );

                group["based_on"] = wmeIds;

                createdGroupIds.push_back(group["id"]);
                this->groups_.push_back(group);
            }
        }

        // handle unannotated wmes
        std::vector<size_t> based_on_wme_ids;
        auto t = token;
        while (t)
        {
            if (wmesInGroups.find(t->wme) == wmesInGroups.end())
            {
                // token groups are processed and connected separately
                if (auto tg = std::dynamic_pointer_cast<TokenGroup>(t->wme))
                {
                    size_t tgId;
                    if (annotation)
                        tgId = processTokenGroup(tg, annotation->parent_);
                    else
                        tgId = processTokenGroup(tg, nullptr);
                    based_on_wme_ids.push_back(tgId);
                }
                else
                {
                    size_t wmeId = getIdOf(t->wme);
                    based_on_wme_ids.push_back(wmeId);
                }
            }
            t = t->parent;
        }
        based_on_wme_ids.insert(
            based_on_wme_ids.end(),
            createdGroupIds.begin(),
            createdGroupIds.end()
        );

        tJSON["based_on"] = based_on_wme_ids;
        this->groups_.push_back(tJSON);
    }

    tgJSON["based_on"] = tokenIDs;
    return tgJSON["id"];
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

    for (auto& entry : processedEffectAnnotations_)
    {
        json.push_back(entry.second);
    }

    for (auto& entry : processedUngroupedEffects_)
    {
        json.push_back(entry.second);
    }

    for (auto& entry : processedTokenGroups_)
    {
        json.push_back(entry.second);
    }

    for (auto& entry : processedAssertions_)
    {
        json.push_back(entry.second);
    }

    return json;
}

void ExplanationToJSONVisitor::clear()
{
    processedAssertions_.clear();
    processedWMEs_.clear();
    groups_.clear();
    lastId_ = 0;
}


}