#ifndef RETE_GROUPBYANNOTATION_HPP_
#define RETE_GROUPBYANNOTATION_HPP_

#include <memory>
#include <vector>
#include "Annotation.hpp"

namespace rete {

/**
 * GROUP BY statement change the layout of the result-tokens of a match:
 * All previous data are put into one TokenGroup-WME, so the token-length
 * shrinks to 1. This also means that we need to re-structure our
 * annotations a bit: All condition-annotations that are created before a
 * GROUP BY refer to the contents of the TokenGroup created by the GROUP BY,
 * and hence should be stored beneath it.
 *
 * This struct is where to store it.
 * And because a GROUP BY can be followed by more conditions and GROUP BYs,
 * it has a place to store a parent.
 */
struct GroupByAnnotation {
    std::vector<Annotation> annotations_;
    std::shared_ptr<GroupByAnnotation> parent_;
};

}

#endif