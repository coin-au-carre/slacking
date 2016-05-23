#ifndef CPR_CPR_TYPES_H
#define CPR_CPR_TYPES_H

#include <map>
#include <string>
#include <algorithm>
#include <cctype>

namespace cpr {

struct CaseInsensitiveCompare {
    bool operator()(const std::string& a, const std::string& b) const noexcept;
};

using Header = std::map<std::string, std::string, CaseInsensitiveCompare>;
using Url = std::string;

bool CaseInsensitiveCompare::operator()(const std::string& a, const std::string& b) const noexcept {
    return std::lexicographical_compare(
            a.begin(), a.end(), b.begin(), b.end(),
            [](unsigned char ac, unsigned char bc) { return std::tolower(ac) < std::tolower(bc); });
}

} // namespace cpr

#endif
