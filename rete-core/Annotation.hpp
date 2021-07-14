#ifndef RETE_PRODUCTION_ANNOTATION_HPP_
#define RETE_PRODUCTION_ANNOTATION_HPP_

#include <string>
#include <map>

#include "Accessors.hpp"

namespace rete {

struct Annotation {
    /**
     * Text annotation of a group of conditions
     */
    std::string annotation_;

    /**
     * Mapping of variable names referenced in the annotation text to
     * accessors to get the respective values from a given token.
     */
    std::map<std::string, AccessorBase::Ptr> variables_;

    /**
     * Begin and end of indices in the full token of WMEs that are
     * grouped here.
     * End is, as usual, one after the last valid index.
     */
    size_t tokenIndexBegin_ = 0;
    size_t tokenIndexEnd_ = 0;

    bool operator == (const Annotation& other)
    {
        return annotation_ == other.annotation_ &&
                tokenIndexBegin_ == other.tokenIndexBegin_&&
                tokenIndexEnd_ == other.tokenIndexEnd_;
    }
};

}

#endif /* RETE_PRODUCTION_ANNOTATION_HPP_ */