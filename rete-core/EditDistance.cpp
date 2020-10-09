#include <vector>
#include <functional>

#include "EditDistance.hpp"

/*
    Implementation taken from:
        http://www.blackbeltcoder.com/Articles/algorithms/approximate-string-comparisons-using-levenshtein-distance
*/
int rete::util::editDistance(
        const std::string& a,
        const std::string& b,
        bool ignoreCase)
{
    // distance matrix
    std::vector<std::vector<int>> d;
    d.resize(a.length()+1);
    for (auto& v : d) v.resize(b.length()+1);

    // comparison function
    std::function<bool(char, char)> equals;
    if (ignoreCase)
    {
        equals = [](char a, char b) -> bool
                 {
                     return std::tolower(a) == std::tolower(b);
                 };
    }
    else
    {
        equals = [](char a, char b) -> bool { return a == b; };
    }

    // compute distance
    for (size_t i = 0; i <= a.length(); i++)
        d[i][0] = i;
    for (size_t j = 0; j <= b.length(); j++)
        d[0][j] = j;
    for (size_t i = 1; i <= a.length(); i++)
    {
        for (size_t j = 1; j <= b.length(); j++)
        {
            if (equals(a[i-1], b[j-1]))
            {
                // no change required
                d[i][j] = d[i-1][j-1];
            }
            else
            {
                d[i][j] =
                    std::min(
                         d[i-1][j] + 1, // deletion
                         std::min(
                             d[i][j-1] + 1, // insertion
                             d[i-1][j-1] + 1)); // substitution
            }
        }
    }

    return d[a.length()][b.length()];
}


