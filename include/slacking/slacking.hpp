#ifndef SLACKING_HPP_
#define SLACKING_HPP_

// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017 Florian Dang
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
#include <mutex>

#include <curl/curl.h>
#include "json.hpp"  // nlohmann/json

 
#ifndef  SLACKING_VERBOSE_OUTPUT
# define SLACKING_VERBOSE_OUTPUT  0
#endif

namespace slack {

namespace _detail {

// Json
using Json = nlohmann::json;

struct Response {
    std::string text;
    bool        is_error;
    std::string error_message;
};


class curl_header {
public: 
    curl_header(CURL *curl) {
        curl_ = curl;
    }
    void append(std::string header) {
        
        element = curl_slist_append(element, header.c_str());

        /* CURLcode rc = curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, element); */
    }

    ~curl_header()  {
        // free the custom headers  
        curl_slist_free_all(element);
    }
    
private:
    CURL *curl_ = nullptr;
    struct curl_slist *element = nullptr;

};

// Simple curl Session inspired by CPR
class Session {
public:
    Session(bool throw_exception) : throw_exception_{throw_exception} {
        curl_global_init(CURL_GLOBAL_ALL);
        curl_ = curl_easy_init();
    }
    Session(bool throw_exception, std::string proxy_url) : throw_exception_{ throw_exception } {
        curl_global_init(CURL_GLOBAL_ALL);
        curl_ = curl_easy_init();
        SetProxyUrl(proxy_url);
    }
    ~Session() { curl_easy_cleanup(curl_); }

    void SetUrl(const std::string& url) { url_ = url;   }

    void SetToken(const std::string &token)
    {
        token_ = token;
    }
 
    void SetProxyUrl(const std::string& url) {
        proxy_url_ = url; 
        if (nullptr != curl_)   curl_easy_setopt(curl_, CURLOPT_PROXY, proxy_url_.c_str());
    }

    void SetBody(const std::string& data);
    Response Get();
    Response Post();
    Response makeRequest();
    std::string easyEscape(const std::string& text);

private:
    static size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
        data->append((char*) ptr, size * nmemb);
        return size * nmemb;
    }

private:
    CURL*       curl_;
    CURLcode    res_;
    std::string url_;
    std::string proxy_url_;
    std::string token_;

    bool        throw_exception_;
    std::mutex  mutex_request_;
};

inline
void Session::SetBody(const std::string& data) { 
        if (curl_) {
            curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, data.length());
            curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data.data());
        }
    }

inline
Response Session::Get() {
    if (curl_) {
        curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl_, CURLOPT_POST, 0L);
        curl_easy_setopt(curl_, CURLOPT_NOBODY, 0L);
    }
    return makeRequest();
}

inline
Response Session::Post() {
    return makeRequest();
}

inline
Response Session::makeRequest() {
    std::lock_guard<std::mutex> lock(mutex_request_);
    
    //------ set our custom set of headers
    // for  using incoming webhook it is mandatory to set 
    // "Content-Type: application/x-www-form-urlencoded" in header

    curl_header  header(curl_);
    header.append("Content-Type: application/x-www-form-urlencoded");
    
    //-------- set our custom set of headers------------------------------  

    curl_easy_setopt(curl_, CURLOPT_URL, url_.c_str());
    
    std::string response_string;
    std::string header_string;
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, writeFunction);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl_, CURLOPT_HEADERDATA, &header_string);

    res_ = curl_easy_perform(curl_);

    bool is_error = false;
    std::string error_msg = "";
    if(res_ != CURLE_OK) {
        is_error = true;
        std::string error_msg = "curl_easy_perform() failed " + std::string{curl_easy_strerror(res_)};
        if (throw_exception_) 
            throw std::runtime_error(error_msg);
        else 
            std::cerr << "[slacking] curl_easy_perform() failed " << error_msg << '\n';
    }

    

    return { response_string, is_error, error_msg };
}

inline
std::string Session::easyEscape(const std::string& text) {
    char *encoded_output = curl_easy_escape(curl_, text.c_str(), static_cast<int>(text.length()));
    return std::string{encoded_output};
}

// forward declaration for category structures
class  Slacking;

struct Channel;
struct User;

// Category structures which mimic Slack API categories

struct CategoryApi {
    // std::string error{}; // not implemented yet
    // std::string foo{};   // not implemented yet

    Json test();

    CategoryApi(Slacking& slack) : slack_{slack} {}
private:
    Slacking& slack_;
};




struct CategoryChannels {
    Json list(bool exclude_archived = false);
    std::vector<Channel> list_magic(bool exclude_archived = false);

    Json info(const std::string& channel_id);

    CategoryChannels(Slacking& slack) : slack_{slack} {}
private:
    Slacking& slack_;
};


// Chat category structure for chat related method such as chat.postMessage. Every public data members can be manually filled.
struct CategoryChat {
    std::string channel{};      // required
    std::string username{};     // optional
    std::string icon_url{};     // optional
    std::string icon_emoji{};   // optional
    std::string parse{};        // optional defaults to none
    bool        as_user{false}; // optional
    Json        attachments{};  // optional should be reset when functor is called
    // unsigned    link_names{1};

    void channel_username_iconemoji(const std::string& c, const std::string& u, const std::string& i) {
        channel = c; username = u; icon_emoji = i;
    }

    Json postMessage(const std::string& text=" ", const std::string& specified_channel="");

    CategoryChat(Slacking& slack) : slack_{slack} {}

public: // exceptional for escape text (should be review)
    Slacking& slack_;
};



struct CategoryWebHook {
    std::string channel{};      // optional
    std::string username{};     // optional
    std::string icon_emoji{};   // optional
    std::string Id{};           // required 

    void channel_username_iconemoji(const std::string& c, const std::string& u, const std::string& i) {
        channel = c; username = u; icon_emoji = i;
    }

    Json postMessage(const std::string& text = " ", const std::string& specified_channel = "");
    
    CategoryWebHook(Slacking& slack) : slack_{ slack } {}

private:
    Slacking& slack_;
};
 


struct CategoryUsers {
    Json list(bool presence = true);
     std::vector<User> list_magic(bool presence = true);

    Json info(const std::string& user_id);

    CategoryUsers(Slacking& slack) : slack_{slack} {}

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


// Basic element types.
struct Element {
    std::string label;
    std::string value;
    bool empty() const { return value.empty(); }
    Element(const std::string& l, const std::string& v) : label(l), value(v) {}
};

std::ostream& operator<<(std::ostream &os, const Element& element);
std::ostream& operator<<(std::ostream &os, const User& user);
std::ostream& operator<<(std::ostream &os, const Channel& channel);

template<typename T>
std::ostream& operator<<(std::ostream &os, const std::vector<T>& vec);

template<class T> void ignore_unused_parameter( const T& ) {}

void remove_first_last_quote(std::string& s);

std::vector<Element> json_to_elements(const Json& json);

template<typename T>
std::string join(const std::vector<T>& vec, const std::string& sep = "&");

void replace_all(std::string& str, const std::string& from, const std::string& to);



class Slacking {
public:
    Slacking() = delete;
    Slacking(const std::string& token, bool throw_exception = true) 
    : session_{throw_exception}, token_{token}, throw_exception_{throw_exception}
    {
        session_.SetUrl("https://slack.com/api/");
        session_.SetToken(token_);
    }
     

    Slacking(const Slacking&)            = delete;
    Slacking& operator=(const Slacking&) = delete;

    void set_proxy(const std::string& url) { session_.SetProxyUrl(url); }


    void change_token(const std::string& token) { token_ = token; };
    void set_throw_exception(bool throw_exception) { throw_exception_ = throw_exception; }

    void trigger_error(const std::string& msg) {
        if (throw_exception_) 
            throw std::runtime_error(msg);
        else 
            std::cerr << "[slacking] error " << msg << '\n';
    }

    Json post(const std::string& method, const std::string& data = "") {
        setParameters(method, data);
        auto response = session_.Post();
        if (response.is_error){ 
            trigger_error(response.error_message);
        }

        Json json{};
        if (isJson(response.text)){

            json = Json::parse(response.text); 
            checkResponse(method, json);
        }
        else{
          #if SLACKING_VERBOSE_OUTPUT
            std::cout << "<< " << response.text << "\n";
          #endif
        }
       
        return json;
    }

    Json get(const std::string& method, const std::string& data = "") {
        setParameters(method, data);
        auto response = session_.Get();
        if (response.is_error) { trigger_error(response.error_message); }

        Json json{};
        if (isJson(response.text)) {
            json = Json::parse(response.text);
            checkResponse(method, json);
        }
        else{
          #if SLACKING_VERBOSE_OUTPUT
            std::cout << "<< " << response.text<< "\n";
          #endif
        }
        return json;
    }

    Json post(const std::string& method, const Json& json) {
        
        auto elements = json_to_elements(json);
        elements.emplace_back("token", token_);
        return post(method, join(elements));
    }

    Json get(const std::string& method, const Json& json) {
        auto elements = json_to_elements(json);
        elements.emplace_back("token", token_);
        return get(method, join(elements));
    }

    std::string easyEscape(const std::string& text) { return session_.easyEscape(text); }

    void debug() const { std::cout << token_ << '\n'; }

    void setBaseUrl(const std::string &url) {
        base_url = url;
    }

    std::string getBaseUrl() const {
        return base_url;
    }
private:
    std::string base_url{ "https://slack.com/api/" };

    void setParameters(const std::string& method, const std::string& data = "") {
        auto complete_url =  base_url+ method;
        session_.SetUrl(complete_url);
        session_.SetBody(data);

#if SLACKING_VERBOSE_OUTPUT
        std::cout << ">> sending: "<< complete_url << "  " << data << '\n';
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
                    // trigger_error(reason);
                    std::cerr << "[slacking] warning! " << method << " checkResponse() failed " << reason << '\n';
#if SLACKING_VERBOSE_OUTPUT
                    std::cerr << json << std::endl;
#endif
                }
                else {
                    trigger_error("checkResponse() unknown error.");
                }
            }
        }
    }

    // as of now the only way
    bool isJson(const std::string &data){
        bool rc = true;
        try {
            auto json = Json::parse(data); // throws if no json 
        }
        catch (std::exception &){
            rc = false;
        }
        return(rc);
    }
private:
    Session    session_;

public:
    std::string         token_;
    bool                throw_exception_;
    CategoryApi         api     {*this};
    CategoryChannels    channels{*this};
    CategoryChat        chat    {*this};
    CategoryUsers       users   {*this};
    CategoryWebHook     hook    {*this};
};

inline 
std::string remove_first_last_quote(std::string str) {
    auto s = str;
    if (s.front() == '"' ) {
        s.erase( 0, 1 );
        s.erase(s.size() - 1);
    }
    return s;
}

inline
auto json_to_elements(const Json& json) -> std::vector<Element> {
    auto elements = std::vector<Element>{};
    elements.reserve(json.size() + 1); // +1 for token
    for (auto it = json.begin(); it != json.end(); ++ it) { 
        std::string label = it.key();
        std::stringstream ss_value;
        ss_value << it.value();
        elements.emplace_back(label, remove_first_last_quote(ss_value.str()));
    }
    return elements;
}

// Free function which looks like python join
template<typename T>
inline
std::string join(const std::vector<T>& vec, const std::string& sep) {
    std::stringstream ss;
    if (vec.size() == 0) { return ""; };
    ss << vec[0];
    for (size_t i = 1; i < vec.size(); i ++) { ss << sep << vec[i]; }
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

inline
std::string bool_to_string(const bool b) {
    std::ostringstream ss;
    ss << std::boolalpha << b;
    return ss.str();
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

// pretty printer for magic output
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
Slacking& create(const std::string& token, bool throw_exception = true)  {
    static Slacking instance(token, throw_exception);
    return instance;
}



inline
Slacking& instance() {
    return create("");
}

inline
Json post(const std::string& method, const Json& json) {
    return instance().post(method, json);
}

inline
Json get(const std::string& method, const Json& json) {
    return instance().get(method, json);
}

// Helper functions to get category structures instance()

inline
CategoryApi& api() {
    return instance().api;
}

inline
CategoryChannels& channels() {
    return instance().channels;
}

inline
CategoryChat& chat() {
    return instance().chat;
}

inline
CategoryUsers& users() {
    return instance().users;
}


// Definitions of category methods

inline
Json CategoryApi::test() {
    auto json = slack_.get("api.test");
    return json;
}


inline
Json CategoryChannels::list(bool exclude_archived) {
    auto exclude_archived_char = exclude_archived ? "1" : "0";
    auto json = slack_.post("channels.list", {{"token", slack_.token_}, {"exclude_archived", exclude_archived_char }});
    return json["channels"];
}

inline
auto CategoryChannels::list_magic(bool exclude_archived) -> std::vector<Channel> {
    auto json_channels = list(exclude_archived);
    auto users = std::vector<Channel>{};
    users.reserve(json_channels.size());
    for (auto channel : json_channels) {
        users.emplace_back(channel["id"], channel["name"], channel["num_members"]);
    }
    return users;
}

inline
Json CategoryChannels::info(const std::string& channel_id) {
    auto json = slack_.post("channels.info", {{"channel", channel_id}});
    return json["channel"];
}

 

inline
Json CategoryChat::postMessage(const std::string& text, const std::string& specified_channel) {
    auto str_channel = specified_channel.empty() ? channel : specified_channel;
    if (str_channel.empty()) { throw std::runtime_error("channel is not set"); }
    auto escaped_text = slack_.easyEscape(text);
    Json json_arguments = {
        { "text"       , escaped_text   }, 
        { "channel"    , str_channel    }, 
        { "username"   , username       },
        { "icon_emoji" , icon_emoji     },
        { "parse"      , parse          },
        { "attachments", attachments    },
        { "as_user"    , bool_to_string(as_user) }
    };
    auto json = slack_.post("chat.postMessage", json_arguments);
    attachments = Json{};
    return json;
}

inline Json CategoryWebHook::postMessage(const std::string& text,
                                         const std::string& specified_channel) {
    auto str_channel = specified_channel.empty() ? channel : specified_channel;

    Json json_arguments = {{"text", text}, {"channel", str_channel}};

    // in this case we did not use  Slack web api
    std::string baseUrl = slack_.getBaseUrl();
    slack_.setBaseUrl("https://hooks.slack.com/services/");

    //  webhooks are used with "Content-Type: application/x-www-form-urlencoded" style
    //  this needs  an escaped payload tag  in  the body
    std::string hookPayload =  "payload=" + slack_.easyEscape(json_arguments.dump());    

    auto  json = slack_.post(Id, hookPayload);

    slack_.setBaseUrl(baseUrl);
    return json;
}

inline
Json CategoryUsers::list(bool presence) {
    auto presence_char = presence ? "1" : "0";
    auto json = slack_.post("users.list", {{"token", slack_.token_}, {"presence", presence_char }});
    return json["members"];
}

inline
auto CategoryUsers::list_magic(bool presence) -> std::vector<User> {
    auto json_members = list(presence);
    auto users = std::vector<User>{};
    users.reserve(json_members.size());
    for (auto member : json_members) {
        auto presence = member.count("presence") ? member["presence"] : "";
        auto email = member["profile"].count("email") ? member["profile"]["email"].dump() : ""; // dump here because email can be null
        bool is_bot = true;
        if (member["is_bot"].is_boolean()) { is_bot = member["is_bot"]; }
        users.emplace_back(member["id"], member["name"], email, member["profile"]["real_name"], presence, is_bot);
    }
    return users;
}

inline
Json CategoryUsers::info(const std::string& user_id) {
    auto json = slack_.post("users.info", {{"user", user_id}});
    return json["user"];
}


} // namespace _detail

// Public interface
using _detail::operator<<;
using _detail::Slacking;

// Meyers' singleton
using _detail::create;
using _detail::instance;

// Generic methods
using _detail::post;
using _detail::get;


// Helper category getters
using _detail::api;
using _detail::channels;
using _detail::chat;
using _detail::users;

using _detail::Json;

} // namespace slack

#endif // SLACKING_HPP_

