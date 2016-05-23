#ifndef CPR_DIGEST_H
#define CPR_DIGEST_H

#include "auth.h"
#include "defines.h"

namespace cpr {

class Digest : public Authentication {
  public:
    template <typename UserType, typename PassType>
    Digest(UserType&& username, PassType&& password)
            : Authentication{CPR_FWD(username), CPR_FWD(password)} {}

    const char* GetAuthString() const noexcept;
};

const char* Digest::GetAuthString() const noexcept {
    return Authentication::GetAuthString();
}


} // namespace cpr

#endif
