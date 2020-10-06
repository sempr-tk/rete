#ifndef RETE_UTIL_HASH_HPP_
#define RETE_UTIL_HASH_HPP_

#include <string>
#include <openssl/evp.h>

namespace rete { namespace util {

/**
    Uses OpenSSL to hash a given string.
*/
class Hash {
public:
    Hash();
    ~Hash();

    std::string digest(const std::string& msg) const;

private:
    EVP_MD_CTX* mdcontext_;
};

}}

#endif /* include guard: RETE_UTIL_HASH_HPP_ */
