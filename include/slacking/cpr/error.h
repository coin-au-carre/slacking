#ifndef CPR_ERROR_H
#define CPR_ERROR_H

#include <cstdint>
#include <string>

#include "cprtypes.h"
#include "defines.h"

#include <curl/curl.h>

namespace cpr {

enum class ErrorCode {
    OK = 0,
    CONNECTION_FAILURE,
    EMPTY_RESPONSE,
    HOST_RESOLUTION_FAILURE,
    INTERNAL_ERROR,
    INVALID_URL_FORMAT,
    NETWORK_RECEIVE_ERROR,
    NETWORK_SEND_FAILURE,
    OPERATION_TIMEDOUT,
    PROXY_RESOLUTION_FAILURE,
    SSL_CONNECT_ERROR,
    SSL_LOCAL_CERTIFICATE_ERROR,
    SSL_REMOTE_CERTIFICATE_ERROR,
    SSL_CACERT_ERROR,
    GENERIC_SSL_ERROR,
    UNSUPPORTED_PROTOCOL,
    UNKNOWN_ERROR = 1000,
};

class Error {
  public:
    Error() : code{ErrorCode::OK} {}

    template <typename TextType>
    Error(const std::int32_t& curl_code, TextType&& p_error_message)
            : code{getErrorCodeForCurlError(curl_code)}, message{CPR_FWD(p_error_message)} {}

    explicit operator bool() const {
        return code != ErrorCode::OK;
    }

    ErrorCode code;
    std::string message;

  private:
    static ErrorCode getErrorCodeForCurlError(std::int32_t curl_code);
};


ErrorCode Error::getErrorCodeForCurlError(std::int32_t curl_code) {
    switch (curl_code) {
        case CURLE_OK:
            return ErrorCode::OK;
        case CURLE_UNSUPPORTED_PROTOCOL:
            return ErrorCode::UNSUPPORTED_PROTOCOL;
        case CURLE_URL_MALFORMAT:
            return ErrorCode::INVALID_URL_FORMAT;
        case CURLE_COULDNT_RESOLVE_PROXY:
            return ErrorCode::PROXY_RESOLUTION_FAILURE;
        case CURLE_COULDNT_RESOLVE_HOST:
            return ErrorCode::HOST_RESOLUTION_FAILURE;
        case CURLE_COULDNT_CONNECT:
            return ErrorCode::CONNECTION_FAILURE;
        case CURLE_OPERATION_TIMEDOUT:
            return ErrorCode::OPERATION_TIMEDOUT;
        case CURLE_SSL_CONNECT_ERROR:
            return ErrorCode::SSL_CONNECT_ERROR;
        case CURLE_PEER_FAILED_VERIFICATION:
            return ErrorCode::SSL_REMOTE_CERTIFICATE_ERROR;
        case CURLE_GOT_NOTHING:
            return ErrorCode::EMPTY_RESPONSE;
        case CURLE_SSL_ENGINE_NOTFOUND:
            return ErrorCode::GENERIC_SSL_ERROR;
        case CURLE_SSL_ENGINE_SETFAILED:
            return ErrorCode::GENERIC_SSL_ERROR;
        case CURLE_SEND_ERROR:
            return ErrorCode::NETWORK_SEND_FAILURE;
        case CURLE_RECV_ERROR:
            return ErrorCode::NETWORK_RECEIVE_ERROR;
        case CURLE_SSL_CERTPROBLEM:
            return ErrorCode::SSL_LOCAL_CERTIFICATE_ERROR;
        case CURLE_SSL_CIPHER:
            return ErrorCode::GENERIC_SSL_ERROR;
        case CURLE_SSL_CACERT:
            return ErrorCode::SSL_CACERT_ERROR;
        case CURLE_USE_SSL_FAILED:
            return ErrorCode::GENERIC_SSL_ERROR;
        case CURLE_SSL_ENGINE_INITFAILED:
            return ErrorCode::GENERIC_SSL_ERROR;
        case CURLE_SSL_CACERT_BADFILE:
            return ErrorCode::SSL_CACERT_ERROR;
        case CURLE_SSL_SHUTDOWN_FAILED:
            return ErrorCode::GENERIC_SSL_ERROR;
        case CURLE_SSL_CRL_BADFILE:
            return ErrorCode::SSL_CACERT_ERROR;
        case CURLE_SSL_ISSUER_ERROR:
            return ErrorCode::SSL_CACERT_ERROR;
        case CURLE_TOO_MANY_REDIRECTS:
            return ErrorCode::OK;
        default:
            return ErrorCode::INTERNAL_ERROR;
    }
}

} // namespace cpr

#endif
