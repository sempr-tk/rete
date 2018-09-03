#include "../include/RuleParserAST.hpp"
#include <ostream>
#include <string>

namespace rete {

namespace ast {

std::ostream& operator << (std::ostream& str, Triple& t)
{
    str << "("
        << t.subject_->value_ << " "
        << t.predicate_->value_ << " "
        << t.object_->value_ << ")";
    return str;
}

} /* ast */

} /* rete */
