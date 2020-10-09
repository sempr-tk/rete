#include "Hash.hpp"

#include <openssl/evp.h>
#include <openssl/opensslv.h>

#include <sstream>
#include <iostream>

/*
    EVP_MD_CTX_new() in 1.1.0 has replaced EVP_MD_CTX_create() in 1.0.x
    EVP_MD_CTX_free() in 1.1.0 has replaced EVP_MD_CTX_destroy() in 1.0.x
    OPENSSL_VERSION_NUMBER =   MMNNFFPPS
                               MM = major
                               | NN = minor
                               |   FF = fix
                               |     PP = patch
                               |       S = status */
#if OPENSSL_VERSION_NUMBER < 0x010100000
    #define EVP_MD_CTX_new() EVP_MD_CTX_create()
    #define EVP_MD_CTX_free(X) EVP_MD_CTX_destroy(X)
#endif

namespace rete { namespace util {

Hash::Hash()
    : mdcontext_(EVP_MD_CTX_new())
{
    if (!mdcontext_) throw std::exception();
}

Hash::~Hash()
{
    EVP_MD_CTX_free(mdcontext_);
}

std::string Hash::digest(const std::string& msg) const
{
    unsigned char* digest = nullptr;
    unsigned int digest_length = 0;

    if (1 != EVP_DigestInit_ex(mdcontext_, EVP_md5(), NULL))
        throw std::exception();
    else if (1 != EVP_DigestUpdate(mdcontext_, msg.c_str(), msg.length()))
        throw std::exception();
    else if (nullptr ==
                (digest = (unsigned char*)
                            OPENSSL_malloc(EVP_MD_size(EVP_md5()))))
        throw std::exception();
    else if (1 != EVP_DigestFinal_ex(mdcontext_, digest, &digest_length))
        throw std::exception();

    std::ostringstream ss;
    ss << std::hex;
    for (unsigned int i = 0; i < digest_length; i++)
    {
        ss << static_cast<unsigned>(digest[i]);
    }

    OPENSSL_free(digest);

    return ss.str();
}

}}
