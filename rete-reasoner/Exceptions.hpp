#ifndef RETE_REASONER_EXCEPTIONS_HPP_
#define RETE_REASONER_EXCEPTIONS_HPP_

#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

#include "../rete-core/EditDistance.hpp"

/**
    This file declares different exceptions that may occur when working with the reasoner.
*/

namespace rete {

/**
    ParserException is the base of most (all?) of the exceptions the rule parser throws. It is
    always because of malformed rule definitions that were given as an input (or at least the
    parsers unability to correctly construct the reasoner/network from the given input).
*/
class ParserException : public std::exception {
protected:
    std::string msg_;
public:
    ParserException() : msg_("ParserException") {}
    ParserException(const std::string& msg) : msg_("ParserException: " + msg) {}
    virtual const char* what() const noexcept override { return msg_.c_str(); }
};


/**
    Adds localisation of the error to an exception
*/
class ParserExceptionLocalized : public ParserException {
    int line_, start_, end_;
public:
    ParserExceptionLocalized() : line_(0), start_(0), end_(0) {}
    ParserExceptionLocalized(const ParserException& ex, int l, int s, int e)
        : line_(l), start_(s), end_(e)
    {
        msg_ = ex.what();
    }

    int row() const { return line_; }
    int colStart() const { return start_; }
    int colEnd() const { return end_; }
};

/**
    The RuleConstructionException takes strings for the currently processed rule,
    condition/effect-part and details of the error to provide debugging information for different
    problems in a uniform manner.
*/
class RuleConstructionException : public ParserException {
protected:
    std::string rule_;
    std::string part_;
    std::string detail_;

    mutable std::string fullerror_;
public:
    RuleConstructionException(const std::string& rule, const std::string& part, const std::string& detail)
        : rule_(rule), part_(part), detail_(detail)
    {
    }

    void setRule(const std::string& rule) { rule_ = rule; }
    void setPart(const std::string& part) { part_ = part; }

    std::string getRule() const { return rule_; }
    std::string getPart() const { return part_; }
    std::string getDetail() const { return detail_; }

    const char* what() const noexcept override
    {
        fullerror_ = "Error while constructing nodes for " + part_ + " in rule: \n    " +
                     rule_ + "\n" + detail_;
        return fullerror_.c_str();
    }
};


/**
    Thrown when there is no registered node builder for the requested type.
*/
class NoBuilderException : public RuleConstructionException {
    std::string unknown_;
    std::vector<std::string> candidates_;
public:
    NoBuilderException(const std::string& rule, const std::string& part, const std::string& type)
        : RuleConstructionException(rule, part, "No builder for node type: " + type), unknown_(type)
    {
    }

    NoBuilderException(const std::string& rule, const std::string& part, const std::string& type, const std::vector<std::string>& knownTypes)
        : RuleConstructionException(rule, part, ""), unknown_(type)
    {
        detail_ = "No builder for node type: " + type + "\n"
                  "Candidates are: \n";

        candidates_ = knownTypes;
        std::sort(candidates_.begin(), candidates_.end());
        std::stable_sort(
            candidates_.begin(),
            candidates_.end(),
            [&type](const std::string& a, const std::string& b) -> bool
            {
                return util::editDistance(a, type) < util::editDistance(b, type);
            });

        for (auto type : candidates_)
        {
            detail_ += "    " + type + "\n";
        }
    }

    std::string bestCandidate() const
    {
        if (candidates_.empty()) return "";
        else return candidates_[0];
    }

    std::string unknown() const
    {
        return unknown_;
    }

};


/**
    The NodeBuilderExceptions should be thrown by NodeBuilders if they encounter any difficulties,
    e.g. unsupported argument types etc. The exception will be be catched by the parser, who adds
    more information about the location where this happened, and rethrows the exception.
*/
class NodeBuilderException : public RuleConstructionException {
public:
    NodeBuilderException(const std::string& detail)
        : RuleConstructionException("", "", detail)
    {
    }
};

} /* rete */


#endif /* end of include guard: RETE_REASONER_EXCEPTIONS_HPP_ */
