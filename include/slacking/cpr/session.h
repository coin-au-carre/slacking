#ifndef CPR_SESSION_H
#define CPR_SESSION_H

#include <cstdint>
#include <memory>

#include <algorithm>
#include <functional>
#include <string>

#include <curl/curl.h>

#include "auth.h"
#include "body.h"
#include "cookies.h"
#include "cprtypes.h"
#include "digest.h"
#include "max_redirects.h"
#include "multipart.h"
#include "parameters.h"
#include "payload.h"
#include "proxies.h"
#include "response.h"
#include "timeout.h"

#include "curlholder.h"
#include "util.h"

namespace cpr {

class Session {
  public:
    Session() : pimpl_{ new Impl{} } {}
    ~Session() {}
    void SetUrl(const Url& url) { pimpl_->SetUrl(url); }
    void SetParameters(const Parameters& parameters) { pimpl_->SetParameters(parameters); }
    void SetParameters(Parameters&& parameters) { pimpl_->SetParameters(std::move(parameters)); }
    void SetHeader(const Header& header) { pimpl_->SetHeader(header); }
    void SetTimeout(const Timeout& timeout) { pimpl_->SetTimeout(timeout); }
    void SetAuth(const Authentication& auth) { pimpl_->SetAuth(auth); }
    void SetDigest(const Digest& auth) { pimpl_->SetDigest(auth); }
    void SetPayload(const Payload& payload) { pimpl_->SetPayload(payload); }
    void SetPayload(Payload&& payload) { pimpl_->SetPayload(std::move(payload)); }
    void SetProxies(const Proxies& proxies) { pimpl_->SetProxies(proxies); }
    void SetProxies(Proxies&& proxies) { pimpl_->SetProxies(std::move(proxies)); }
    void SetMultipart(const Multipart& multipart) { pimpl_->SetMultipart(multipart); }
    void SetMultipart(Multipart&& multipart) { pimpl_->SetMultipart(std::move(multipart)); }
    void SetRedirect(const bool& redirect) { pimpl_->SetRedirect(redirect); }
    void SetMaxRedirects(const MaxRedirects& max_redirects) { pimpl_->SetMaxRedirects(max_redirects); }
    void SetCookies(const Cookies& cookies) { pimpl_->SetCookies(cookies); }
    void SetBody(const Body& body) { pimpl_->SetBody(body); }
    void SetBody(Body&& body) { pimpl_->SetBody(std::move(body)); }
    void SetOption(const Url& url) { pimpl_->SetUrl(url); }
    void SetOption(const Parameters& parameters) { pimpl_->SetParameters(parameters); }
    void SetOption(Parameters&& parameters) { pimpl_->SetParameters(std::move(parameters)); }
    void SetOption(const Header& header) { pimpl_->SetHeader(header); }
    void SetOption(const Timeout& timeout) { pimpl_->SetTimeout(timeout); }
    void SetOption(const Authentication& auth) { pimpl_->SetAuth(auth); }
    void SetOption(const Digest& auth) { pimpl_->SetDigest(auth); }
    void SetOption(const Payload& payload) { pimpl_->SetPayload(payload); }
    void SetOption(Payload&& payload) { pimpl_->SetPayload(std::move(payload)); }
    void SetOption(const Proxies& proxies) { pimpl_->SetProxies(proxies); }
    void SetOption(Proxies&& proxies) { pimpl_->SetProxies(std::move(proxies)); }
    void SetOption(const Multipart& multipart) { pimpl_->SetMultipart(multipart); }
    void SetOption(Multipart&& multipart) { pimpl_->SetMultipart(std::move(multipart)); }
    void SetOption(const bool& redirect) { pimpl_->SetRedirect(redirect); }
    void SetOption(const MaxRedirects& max_redirects) { pimpl_->SetMaxRedirects(max_redirects); }
    void SetOption(const Cookies& cookies) { pimpl_->SetCookies(cookies); }
    void SetOption(const Body& body) { pimpl_->SetBody(body); }
    void SetOption(Body&& body) { pimpl_->SetBody(std::move(body)); }
    Response Delete() { return pimpl_->Delete(); }
    Response Get() { return pimpl_->Get(); }
    Response Head() { return pimpl_->Head(); }
    Response Options() { return pimpl_->Options(); }
    Response Patch() { return pimpl_->Patch(); }
    Response Post() { return pimpl_->Post(); }
    Response Put() { return pimpl_->Put(); }

  private:
    class Impl {
      public:
        Impl();

        void SetUrl(const Url& url);
        void SetParameters(const Parameters& parameters);
        void SetParameters(Parameters&& parameters);
        void SetHeader(const Header& header);
        void SetTimeout(const Timeout& timeout);
        void SetAuth(const Authentication& auth);
        void SetDigest(const Digest& auth);
        void SetPayload(Payload&& payload);
        void SetPayload(const Payload& payload);
        void SetProxies(Proxies&& proxies);
        void SetProxies(const Proxies& proxies);
        void SetMultipart(Multipart&& multipart);
        void SetMultipart(const Multipart& multipart);
        void SetRedirect(const bool& redirect);
        void SetMaxRedirects(const MaxRedirects& max_redirects);
        void SetCookies(const Cookies& cookies);
        void SetBody(Body&& body);
        void SetBody(const Body& body);

        Response Delete();
        Response Get();
        Response Head();
        Response Options();
        Response Patch();
        Response Post();
        Response Put();

      private:
        std::unique_ptr<CurlHolder, std::function<void(CurlHolder*)>> curl_;
        Url url_;
        Parameters parameters_;
        Proxies proxies_;

        Response makeRequest(CURL* curl);
        static void freeHolder(CurlHolder* holder);
        static CurlHolder* newHolder();
    };

    std::unique_ptr<Impl> pimpl_;
};


Session::Impl::Impl() {
    curl_ = std::unique_ptr<CurlHolder, std::function<void(CurlHolder*) >>(newHolder(),
                                                                           &Impl::freeHolder);
    auto curl = curl_->handle;
    if (curl) {
        // Set up some sensible defaults
        auto version_info = curl_version_info(CURLVERSION_NOW);
        auto version = std::string{"curl/"} + std::string{version_info->version};
        curl_easy_setopt(curl, CURLOPT_USERAGENT, version.data());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_->error);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
#ifdef INSECURE_CURL
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
#if LIBCURL_VERSION_MAJOR >= 7
#if LIBCURL_VERSION_MINOR >= 25
#if LIBCURL_VERSION_PATCH >= 0
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
#endif
#endif
#endif
    }
}

void Session::Impl::freeHolder(CurlHolder* holder) {
    curl_easy_cleanup(holder->handle);
    curl_slist_free_all(holder->chunk);
    delete holder;
}

CurlHolder* Session::Impl::newHolder() {
    CurlHolder* holder = new CurlHolder();
    holder->handle = curl_easy_init();
    return holder;
}

void Session::Impl::SetUrl(const Url& url) {
    url_ = url;
}

void Session::Impl::SetParameters(const Parameters& parameters) {
    parameters_ = parameters;
}

void Session::Impl::SetParameters(Parameters&& parameters) {
    parameters_ = std::move(parameters);
}

void Session::Impl::SetHeader(const Header& header) {
    auto curl = curl_->handle;
    if (curl) {
        struct curl_slist* chunk = NULL;
        for (auto item = header.cbegin(); item != header.cend(); ++item) {
            auto header_string = std::string{item->first};
            if (item->second.empty()) {
                header_string += ";";
            } else {
                header_string += ": " + item->second;
            }
            chunk = curl_slist_append(chunk, header_string.data());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_->chunk = chunk;
        }
    }
}

void Session::Impl::SetTimeout(const Timeout& timeout) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout.ms);
    }
}

void Session::Impl::SetAuth(const Authentication& auth) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERPWD, auth.GetAuthString());
    }
}

void Session::Impl::SetDigest(const Digest& auth) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
        curl_easy_setopt(curl, CURLOPT_USERPWD, auth.GetAuthString());
    }
}

void Session::Impl::SetPayload(Payload&& payload) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload.content.length());
        curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, payload.content.data());
    }
}

void Session::Impl::SetPayload(const Payload& payload) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload.content.length());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.content.data());
    }
}

void Session::Impl::SetProxies(const Proxies& proxies) {
    proxies_ = proxies;
}

void Session::Impl::SetProxies(Proxies&& proxies) {
    proxies_ = std::move(proxies);
}

void Session::Impl::SetMultipart(Multipart&& multipart) {
    auto curl = curl_->handle;
    if (curl) {
        struct curl_httppost* formpost = NULL;
        struct curl_httppost* lastptr = NULL;

        for (auto& part : multipart.parts) {
            auto content_option = CURLFORM_COPYCONTENTS;
            if (part.is_file) {
                content_option = CURLFORM_FILE;
            }
            if (part.content_type.empty()) {
                curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, part.name.data(),
                             content_option, part.value.data(), CURLFORM_END);
            } else {
                curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, part.name.data(),
                             content_option, part.value.data(), CURLFORM_CONTENTTYPE,
                             part.content_type.data(), CURLFORM_END);
            }
        }
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    }
}

void Session::Impl::SetMultipart(const Multipart& multipart) {
    auto curl = curl_->handle;
    if (curl) {
        struct curl_httppost* formpost = NULL;
        struct curl_httppost* lastptr = NULL;

        for (auto& part : multipart.parts) {
            auto content_option = CURLFORM_PTRCONTENTS;
            if (part.is_file) {
                content_option = CURLFORM_FILE;
            }
            if (part.content_type.empty()) {
                curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, part.name.data(),
                             content_option, part.value.data(), CURLFORM_END);
            } else {
                curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, part.name.data(),
                             content_option, part.value.data(), CURLFORM_CONTENTTYPE,
                             part.content_type.data(), CURLFORM_END);
            }
        }
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    }
}

void Session::Impl::SetRedirect(const bool& redirect) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, std::int32_t(redirect));
    }
}

void Session::Impl::SetMaxRedirects(const MaxRedirects& max_redirects) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, max_redirects.number_of_redirects);
    }
}

void Session::Impl::SetCookies(const Cookies& cookies) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
        curl_easy_setopt(curl, CURLOPT_COOKIE, cookies.GetEncoded().data());
    }
}

void Session::Impl::SetBody(Body&& body) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
        curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, body.data());
    }
}

void Session::Impl::SetBody(const Body& body) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.data());
    }
}

Response Session::Impl::Delete() {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 0L);
        curl_easy_setopt(curl, CURLOPT_POST, 0L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }

    return makeRequest(curl);
}

Response Session::Impl::Get() {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_POST, 0L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
    }

    return makeRequest(curl);
}

Response Session::Impl::Head() {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 0L);
        curl_easy_setopt(curl, CURLOPT_POST, 0L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    }

    return makeRequest(curl);
}

Response Session::Impl::Options() {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 0L);
        curl_easy_setopt(curl, CURLOPT_POST, 0L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "OPTIONS");
    }

    return makeRequest(curl);
}

Response Session::Impl::Patch() {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    }

    return makeRequest(curl);
}

Response Session::Impl::Post() {
    return makeRequest(curl_->handle);
}

Response Session::Impl::Put() {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    }

    return makeRequest(curl);
}

Response Session::Impl::makeRequest(CURL* curl) {
    if (!parameters_.content.empty()) {
        Url new_url{url_ + "?" + parameters_.content};
        curl_easy_setopt(curl, CURLOPT_URL, new_url.data());
    } else {
        curl_easy_setopt(curl, CURLOPT_URL, url_.data());
    }

    auto protocol = url_.substr(0, url_.find(':'));
    if (proxies_.has(protocol)) {
        curl_easy_setopt(curl, CURLOPT_PROXY, proxies_[protocol].data());
    } else {
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
    }

    curl_->error[0] = '\0';

    std::string response_string;
    std::string header_string;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cpr::util::writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

    auto curl_error = curl_easy_perform(curl);

    char* raw_url;
    long response_code;
    double elapsed;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &raw_url);

    Error error(curl_error, curl_->error);

    Cookies cookies;
    struct curl_slist* raw_cookies;
    curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &raw_cookies);
    for (struct curl_slist* nc = raw_cookies; nc; nc = nc->next) {
        auto tokens = cpr::util::split(nc->data, '\t');
        auto value = tokens.back();
        tokens.pop_back();
        cookies[tokens.back()] = value;
    }
    curl_slist_free_all(raw_cookies);

    auto header = cpr::util::parseHeader(header_string);
    response_string = cpr::util::parseResponse(response_string);
    return Response{static_cast<std::int32_t>(response_code),
                    response_string,
                    header,
                    raw_url,
                    elapsed,
                    cookies,
                    error};
}

} // namespace cpr

#endif
