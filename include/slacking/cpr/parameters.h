#ifndef CPR_PARAMETERS_H
#define CPR_PARAMETERS_H

#include <initializer_list>
#include <memory>
#include <string>
#include <string>

#include "defines.h"
#include "util.h"

namespace cpr {

struct Parameter {
    template <typename KeyType, typename ValueType>
    Parameter(KeyType&& key, ValueType&& value)
            : key{CPR_FWD(key)}, value{CPR_FWD(value)} {}

    std::string key;
    std::string value;
};

class Parameters {
  public:
    Parameters() = default;
    Parameters(const std::initializer_list<Parameter>& parameters);

    void AddParameter(const Parameter& parameter);

    std::string content;
};

Parameters::Parameters(const std::initializer_list<Parameter>& parameters) {
    for (const auto& parameter : parameters) {
        AddParameter(parameter);
    }
}

void Parameters::AddParameter(const Parameter& parameter) {
    if (!content.empty()) {
        content += "&";
    }

    auto escapedKey = cpr::util::urlEncode(parameter.key);
    if (parameter.value.empty()) {
        content += escapedKey;
    } else {
        auto escapedValue = cpr::util::urlEncode(parameter.value);
        content += escapedKey + "=" + escapedValue;
    }
}


} // namespace cpr

#endif
