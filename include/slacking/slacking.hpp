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

namespace _detail {

using Json = nlohmann::json;
using zstring   = char *;
using zwstring  = wchar_t *;
using czstring  = const char *;
using cwzstring = const wchar_t * ;

// forward declaration for magic structures
class Slacking; 
struct Element;

// Magic structure for chat.postMessage every public data members can be manually filled.
struct Magic_chat_postMessage {
    std::string channel{};    // required
    std::string username{};   // optional
    std::string icon_emoji{}; // optional
    std::string parse{};      // optional defaults to none
    std::string attachments{}; // optional should be reset when functor is called

    void channel_username_iconemoji(const std::string& c, const std::string& u, const std::string& i) {
        channel = c; username = u; icon_emoji = i;
    }

    Json operator()(std::string text=" ", const std::string& specified_channel="");
    Magic_chat_postMessage(Slacking& slack) : slack_{slack} {}

private:
    Slacking& slack_;
};

// User convenient structure for users.list
struct User {
    std::string id;
    std::string name;
    std::string email;
    std::string real_name;
    std::string presence;
    bool is_bot; // someone who left the team is considered as bot? and slackbot is considered as human?

    User(const std::string& i, const std::string& n, const std::string& e, const std::string& r, const std::string& p, bool bot) 
        : id{i}, name{n}, email{e}, real_name{r}, presence{p}, is_bot{bot} {}
};

// Channel convenient structure for channels.list
struct Channel {
    std::string id;
    std::string name;
    unsigned num_members;

    Channel(const std::string& i, const std::string& n, unsigned num) 
        : id{i}, name{n}, num_members{num} {}
};


// Basic element types. Uses czstring so that it is a POD.
struct Element {
    czstring label;
    czstring value;
    bool empty() const { return value && !value[0]; }
};

std::ostream& operator<<(std::ostream &os, const Element& element);
std::ostream& operator<<(std::ostream &os, const User& user);
std::ostream& operator<<(std::ostream &os, const Channel& channel);

template<typename T>
std::ostream& operator<<(std::ostream &os, const std::vector<T>& vec);

template<class T> void ignore_unused_parameter( const T& ) {}

template<typename T>
std::string join(const std::vector<T>& vec, const std::string sep = "&");

template<typename T>
std::string join_non_empty(const std::vector<T>& vec, const std::string sep = "&");

void replace_all(std::string& str, const std::string& from, const std::string& to);

void replace_slack_escape_characters(std::string& str);


class Slacking {
public:
    Slacking() = delete;
    Slacking(const std::string& token) : session_{}, token_{token} {
        session_.SetUrl("https://slack.com/api/");
        // session_.SetTimeout(cpr::Timeout{700});
    }

    Slacking(const Slacking&)            = delete;
    Slacking& operator=(const Slacking&) = delete;

    void reset_token(const std::string& token) { token_ = token; };

    Json post(const std::string& method, const std::string& data = "") {
        setParameters(method, data);
        auto response = session_.Post();
        if (response.error) { throw std::runtime_error(response.error.message); }
        auto json = Json::parse(response.text);
        checkResponse(method, json);
        return json;
    }

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

    Json get(const std::string& method, std::vector<Element> elements) {
        elements.emplace_back(Element{"token", token_.c_str()});
        return get(method, join(elements));
    }

    Json users_list(bool presence = true) {
        auto presence_char = presence ? "1" : "0";
        auto json = post("users.list", {{"token", token_.c_str()}, {"presence", presence_char }});
        return json["members"];
    }

    std::vector<User> magic_users_list(bool presence = true) {
        auto json_members = users_list(presence);
        auto users = std::vector<User>{};
        users.reserve(json_members.size());
        for (auto member : json_members) {
            auto presence = member.count("presence") ? member["presence"] : "";
            auto email = member["profile"].count("email") ? member["profile"]["email"].dump() : ""; // dump here because email can be null
            bool is_bot = true;
            if (member["is_bot"].is_boolean()) { is_bot = member["is_bot"]; }
            users.push_back(User{member["id"], member["name"], email, member["profile"]["real_name"], presence, is_bot});
        }
        return users;
    }

    Json users_info(const std::string& user_id) {
        auto json = post("users.info", {{"user", user_id.c_str()}});
        return json["user"];
    }

    Json channels_list(bool exclude_archived = false) {
        auto exclude_archived_char = exclude_archived ? "1" : "0";
        auto json = post("channels.list", {{"token", token_.c_str()}, {"exclude_archived", exclude_archived_char }});
        return json["channels"];
    }

    std::vector<Channel> magic_channels_list(bool exclude_archived = false) {
        auto json_channels = channels_list(exclude_archived);
        auto users = std::vector<Channel>{};
        users.reserve(json_channels.size());
        for (auto channel : json_channels) {
            users.push_back(Channel{channel["id"], channel["name"], channel["num_members"]});
        }
        return users;
    }

    Json channels_info(const std::string& channel_id) {
        auto json = post("channels.info", {{"channel", channel_id.c_str()}});
        return json["channel"];
    }

    void api_test() { auto json = get("api.test"); } // If no error is thrown then everything is ok

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
        ignore_unused_parameter(method);
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

public:
    Magic_chat_postMessage chat_postMessage{*this};

private:
    cpr::Session    session_;
    std::string     token_;
};

// Free function which looks like python join
template<typename T>
inline
std::string join(const std::vector<T>& vec, const std::string sep) {
    std::stringstream ss;
    if (vec.size() == 0) { return ""; };
    ss << vec[0];
    for (size_t i = 1; i < vec.size(); i ++) { ss << sep << vec[i]; }
    return ss.str();
}

template<typename T>
inline
std::string join_non_empty(const std::vector<T>& vec, const std::string sep) {
    std::stringstream ss;
    if (vec.size() == 0) { return ""; };
    ss << vec[0];
    for (size_t i = 1; i < vec.size(); i ++) { 
        if (!vec[i].empty()) {
            ss << sep << vec[i];
        } 
    }
    return ss.str();
}

inline
void replace_all(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty()) { return; }
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

// https://api.slack.com/docs/formatting
inline
void replace_slack_escape_characters(std::string& str) {
    replace_all(str, "&", "&amp;");
    replace_all(str, "<", "&lt;");
    replace_all(str, ">", "&gt;");
}

inline
Json Magic_chat_postMessage::operator()(std::string text, const std::string& specified_channel) {
    auto str_channel = specified_channel.empty() ? channel : specified_channel;
    if (str_channel.empty()) { throw std::invalid_argument("channel is not set"); }
    // replace_slack_escape_characters(text); // No need CPR does the work and supports Url encoded POST values
    auto elements = std::vector<Element>{
        { "text"      , text.c_str()         }, 
        { "channel"   , str_channel.c_str()  }, 
        { "username"  , username.c_str()     },
        { "icon_emoji", icon_emoji.c_str()   },
        { "parse"     , parse.c_str()        },
        { "attachments", attachments.c_str() }
    };
    auto json = slack_.post("chat.postMessage", elements);
    attachments = std::string{}; // reset
    return json;
}


inline
std::ostream& operator<<(std::ostream &os, const Element& element) {
    return os << element.label << '=' << element.value;
}

inline
std::ostream& operator<<(std::ostream &os, const User& user) {
    std::string bot_str = user.is_bot ? "Bot" : "Human";
    return os << "{ " << join(std::vector<std::string>{user.id, user.name, user.real_name, user.email, user.presence, bot_str}, ", ") << " }";
}

inline
std::ostream& operator<<(std::ostream &os, const Channel& channel) {
    return os << "{ " << join(std::vector<std::string>{channel.id, channel.name, std::to_string(channel.num_members)}, ", ") << " }";
}

template<typename T>
inline
std::ostream& operator<<(std::ostream &os, const std::vector<T>& vec) {
    os << '[';
    for(auto const& v : vec) {
        os << "\n  " << v << ',';
    }
    return os << "\b\n]";
}


inline
Slacking& create(const std::string& token)  {
    static Slacking instance(token);
    return instance;
}

inline
Slacking& instance() {
    return create("");
}

inline
void api_test() {
    instance().api_test();
}

inline
Json chat_postMessage(std::string text=" ", const std::string& specified_channel="") {
    return instance().chat_postMessage(text, specified_channel);
}

inline
void channel_username_iconemoji(const std::string& c, const std::string& u, const std::string& i) {
    return instance().chat_postMessage.channel_username_iconemoji(c,u,i);
}

inline
void set_attachments(const Json& json) {
    instance().chat_postMessage.attachments = json.dump();
}


inline
Json users_list(bool presence = true) {
    return instance().users_list(presence);
}

inline
auto magic_users_list(bool presence = true) -> std::vector<User> {
    return instance().magic_users_list(presence);
}

inline
Json users_info(const std::string& user_id) {
    return instance().users_info(user_id);
}

inline
Json channels_list(bool exclude_archived = false) {
    return instance().channels_list(exclude_archived);
}

inline
auto magic_channels_list(bool exclude_archived = false) -> std::vector<Channel> {
    return instance().magic_channels_list(exclude_archived);
}

inline
Json channels_info(const std::string& channels_id) {
    return instance().channels_info(channels_id);
}

inline
void post(const std::string& method, std::vector<Element> elements) {
    instance().post(method, elements);
}

inline
void get(const std::string& method, std::vector<Element> elements) {
    instance().get(method, elements);
}

} // namespace _detail

// Public interface
using _detail::Slacking;

using _detail::create;
using _detail::instance;

using _detail::post;
using _detail::get;
using _detail::operator<<;

using _detail::api_test;

using _detail::chat_postMessage;
using _detail::channel_username_iconemoji;
using _detail::set_attachments;

using _detail::users_list;
using _detail::magic_users_list;
using _detail::users_info;

using _detail::channels_list;
using _detail::magic_channels_list;
using _detail::channels_info;


} // namespace slack

#endif // SLACKING_HPP_

