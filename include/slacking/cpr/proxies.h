#ifndef CPR_PROXIES_H
#define CPR_PROXIES_H

#include <initializer_list>
#include <map>
#include <string>
#include <utility>

namespace cpr {

class Proxies {
  public:
    Proxies() {}
    Proxies(const std::initializer_list<std::pair<const std::string, std::string>>& hosts);

    bool has(const std::string& protocol) const;
    const std::string& operator[](const std::string& protocol);

  private:
    std::map<std::string, std::string> hosts_;
};

Proxies::Proxies(const std::initializer_list<std::pair<const std::string, std::string>>& hosts)
        : hosts_{hosts} {}

bool Proxies::has(const std::string& protocol) const {
    return hosts_.count(protocol) > 0;
}

const std::string& Proxies::operator[](const std::string& protocol) {
    return hosts_[protocol];
}

} // namespace cpr

#endif
