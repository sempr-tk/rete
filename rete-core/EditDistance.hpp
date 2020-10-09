#include <string>

namespace rete {
namespace util {

/**
    Computes the Levenshtein distance between two strings: The number of edits
    needed to convert one string to the other.

    \param a string 1
    \param b string 2
    \param ignoreCase indicates if comparisons should be made without regard to
*/
int editDistance(
        const std::string& a,
        const std::string& b,
        bool ignoreCase = false);


}}
