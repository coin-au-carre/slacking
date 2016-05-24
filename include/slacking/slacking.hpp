#ifndef SLACKING_HPP_
#define SLACKING_HPP_

// The MIT License (MIT)
// 
// Copyright (c) 2016 Florian Dang
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>

#include "cpr/cpr.h"      // whoshuu/cpr (modified header only version)
#include "json/json.hpp"  // nlohmann/json

#ifndef  SLACKING_VERBOSE_OUTPUT
# define SLACKING_VERBOSE_OUTPUT  0
#endif

namespace slack {

using Json = nlohmann::json;
using zstring   = char *;
using zwstring  = wchar_t *;
using czstring  = const char *;
using cwzstring = const wchar_t * ;

// Free function which looks like python join
template<typename T>
std::string join(const std::vector<T>& vec, const std::string sep = "&") {
    std::stringstream ss;
    if (vec.size() == 0) { return ""; };
    ss << vec[0];
    for (size_t i = 1; i < vec.size(); i ++) { ss << sep << vec[i]; }
    return ss.str();
}

// Basic element types. Uses czstring so that it is a POD type.
struct Element {
    czstring label;
    czstring value;
};


// User convenient structure for users.list
struct User {
    using string = std::string;
    User(bool i, string n, string e, string r, string p) : is_bot{i}, name{n}, email{e}, real_name{r}, presence{p} {}
    bool   is_bot; // someone who left the team is considered as bot? and slackbot is considered as human?
    string name;
    string email;
    string real_name;
    string presence;
};


std::ostream & operator<<(std::ostream &os, const Element& element);
std::ostream & operator<<(std::ostream &os, const User& user);
std::ostream & operator<<(std::ostream &os, const std::vector<User>& users);

class Slacking {
public:
    Slacking() = delete;
    Slacking(const std::string& token, const std::string& channel = ""
        , const std::string& username="Slacking bot", const std::string& icon_emoji="") 
    : session_{}, token_{token}, channel_{channel}, username_{username}, icon_emoji_{icon_emoji} {
        session_.SetUrl("https://slack.com/api/");
        // session_.SetTimeout(cpr::Timeout{700});
    }

    Slacking(const Slacking&)            = delete;
    Slacking& operator=(const Slacking&) = delete;

    void set_token(const std::string& token)            { token_ = token; };
    void set_channel(const std::string& channel)        { channel_ = channel; };
    void set_username(const std::string& username)      { username_ = username; };
    void set_icon_emoji(const std::string& icon_emoji)  { icon_emoji_ = icon_emoji; };

    //! Post method
    Json post(const std::string& method, const std::string& data = "") {
        setParameters(method, data);
        auto response = session_.Post();
        if (response.error) { throw std::runtime_error(response.error.message); }
        auto json = Json::parse(response.text);
        checkResponse(method, json);
        return json;
    }

    //! Get method
    Json get(const std::string& method, const std::string& data = "") {
        setParameters(method, data);
        auto response = session_.Get();
        if (response.error) { throw std::runtime_error(response.error.message); }
        auto json = Json::parse(response.text);
        checkResponse(method, json);
        return json;
    }

    Json post(const std::string& method, std::vector<Element> elements) {
        elements.emplace_back(Element{"token", token_.c_str()});
        return post(method, join(elements));
    }

    Json get(const std::string& method, std::vector<Element>&elements) {
        elements.emplace_back(Element{"token", token_.c_str()});
        return get(method, join(elements));
    }

    // curl -XPOST "https://slack.com/api/chat.postMessage" -d 'token=???&channel=#thevoid&text=coincoin' 
    void chat_postMessage(const std::string& text) {
        auto elements = std::vector<Element>{
            { "channel", channel_.c_str() }, 
            { "text", text.c_str() }, 
            { "username",username_.c_str() },
            { "icon_emoji",icon_emoji_.c_str() }
        };
        auto json = post("chat.postMessage", elements);
    }

    Json users_list(bool presence = true) {
        auto presence_char = presence ? "1" : "0";
        auto json = post("users.list", {{"token", token_.c_str()}, {"presence", presence_char }});
        return json["members"];
    }

    std::vector<User> users_list_magic(bool presence = true) {
        auto json_members = users_list(true);
        auto users = std::vector<User>{};
        users.reserve(json_members.size());
        for (auto member : json_members) {
            auto presence = member.count("presence") ? member["presence"].dump() : "";
            auto email = member["profile"].count("email") ? member["profile"]["email"].dump() : "";
            bool is_bot = true;
            if (member["is_bot"].is_boolean()) { is_bot = member["is_bot"]; }
            users.push_back(User{is_bot, member["name"].dump(), email, member["profile"]["real_name"].dump(), presence});
        }
        return users;
    }

    void apiTest() { auto json = get("api.test"); }

    void debug() const { std::cout << token_ << std::endl; }

    
private:
    void setParameters(const std::string& method, const std::string& data = "") {
        auto complete_url = "https://slack.com/api/" + method;
        session_.SetUrl(complete_url);
        session_.SetBody(cpr::Body{data});
#if SLACKING_VERBOSE_OUTPUT
        std::cout << ">> sending: "<< complete_url << "  " << data << std::endl;
#endif
    }

    void checkResponse(const std::string& method, const Json& json) {
        if (json.count("ok")) {
            if(json["ok"].dump() == "true") {
#if SLACKING_VERBOSE_OUTPUT
                std::cout << "<< " << method << " [passed]\n";
#endif
            }
            else {
                if (json.count("error")) {
                    auto reason = json["error"].dump();
#if SLACKING_VERBOSE_OUTPUT
                    std::cerr << "<< Error! " << method << " call [failed] Reason given: " << reason << std::endl;
#endif
                    throw std::runtime_error(reason);
                }
                throw std::runtime_error("checkResponse() unknown error.");
            }
        }
    }

private:
    cpr::Session    session_;
    std::string     token_;
    std::string     channel_;
    std::string     username_;
    std::string     icon_emoji_;
};

inline
Slacking& createInstance(   const std::string& token, 
                            const std::string& channel = "", 
                            const std::string& username = "", 
                            const std::string& icon_emoji = "")  {
    static Slacking instance(token,channel,username,icon_emoji);
    return instance;
}

inline
std::ostream & operator<<(std::ostream &os, const Element& element) {
    return os << element.label << '=' << element.value;
}

inline
std::ostream & operator<<(std::ostream &os, const User& user) {
    std::string bot_str = user.is_bot ? "Bot" : "Human";
    return os << "{ " << join(std::vector<std::string>{user.name, user.real_name, user.email, bot_str, user.presence}, ", ") << " }";
}

inline
std::ostream & operator<<(std::ostream &os, const std::vector<User>& users) {
    os << '[';
    for(auto const& user : users) {
        os << "\n  " << user << ',';
    }
    return os << "\b\n]";
}


inline
Slacking& instance() {
    return createInstance("");
}

inline
void apiTest() {
    instance().apiTest();
}

inline
void chat_postMessage(const std::string& text) {
    instance().chat_postMessage(text);
}

inline
Json users_list(bool presence = true) {
    return instance().users_list(presence);
}

inline
void post(const std::string& method, std::vector<Element> elements) {
    instance().post(method, elements);
}

} // namespace slack

#endif // SLACKING_HPP_

