#include "../include/RuleParserAST.hpp"
#include <ostream>
#include <string>
#include <algorithm>

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


bool subst(std::string& str, const std::string& pre, const std::string& subst)
{
    if (str.size() < pre.size()) return false;

    auto match = std::mismatch(pre.begin(), pre.end(), str.begin());
    if (match.first == pre.end())
    {
        // pre is prefix of str
        str.replace(str.begin(), match.second, subst.begin(), subst.end());
        // add <...>
        str.insert(0, "<");
        str.push_back('>');
        return true;
    }
    return false;
}


void Triple::substitutePrefixes(std::map<std::string, std::string>& prefixes)
{
    bool s, p, o;
    s = p = o = false;

    for (auto entry : prefixes)
    {
        if (!s) s = subst(subject_->value_, entry.first, entry.second);
        if (!p) p = subst(predicate_->value_, entry.first, entry.second);
        if (!o) o = subst(object_->value_, entry.first, entry.second);
        if (s && p && o) break;
    }
}




} /* ast */

} /* rete */
