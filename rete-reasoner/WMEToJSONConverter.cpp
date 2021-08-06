#include "WMEToJSONConverter.hpp"
#include <nlohmann/json.hpp>

#include "../rete-rdf/Triple.hpp"

namespace rete {


bool TripleToJSONConverter::convert(WME::Ptr wme, std::string& json)
{
    auto triple = std::dynamic_pointer_cast<Triple>(wme);
    if (!triple) return false;

    nlohmann::json j;
    j["type"] = "triple";
    j["description"] = triple->description_;
    j["value"]["subject"] = triple->getField(Triple::SUBJECT);
    j["value"]["predicate"] = triple->getField(Triple::PREDICATE);
    j["value"]["object"] = triple->getField(Triple::OBJECT);

    json = j.dump();
    return true;
}


bool DefaultToJSONConverter::convert(WME::Ptr wme, std::string& json)
{
    nlohmann::json j;
    j["type"] = "unknown";
    j["value"] = wme->toString();
    j["description"] = wme->description_;

    json = j.dump();
    return true;
}


}