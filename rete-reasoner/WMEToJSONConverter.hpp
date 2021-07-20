#ifndef RETE_WMETOJSONCONVERTER_HPP_
#define RETE_WMETOJSONCONVERTER_HPP_

#include "../rete-core/WME.hpp"

namespace rete {

/**
 * A base object for conversion of WMEs to a JSON format representing its content.
 * This is not used for persistence, but for creating explanation graphs!
 */
class WMEToJSONConverter {
public:
    virtual ~WMEToJSONConverter() = default;

    /**
     * Convert a WME to its json representation. Returns true if the wme was
     * accepted and converted, false otherwise. std::string is chosen as the
     * interface type to avoid conflicts with possibly different versions of
     * the used json library in different projects.
     */
    virtual bool convert(WME::Ptr wme, std::string& json) = 0;
};



/**
 * Implementation of WMEToJSONConverter for triples
 */
class TripleToJSONConverter : public WMEToJSONConverter {
public:
    bool convert(WME::Ptr wme, std::string& json) override;
};


/**
 * Default-implementation of WMEToJSONConverter, simply uses toString() of the
 * WME as a backup.
 */
class DefaultToJSONConverter : public WMEToJSONConverter {
public:
    bool convert(WME::Ptr wme, std::string& json) override;
};


}

#endif /* RETE_WMETOJSONCONVERTER_HPP_ */