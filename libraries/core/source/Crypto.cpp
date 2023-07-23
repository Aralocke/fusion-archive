#include <Fusion/Crypto.h>

#include <Fusion/Macros.h>

#include <openssl/base.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

#include <ctime>

namespace Fusion
{
template<>
void Deleter::operator()(BIO* ptr) const
{
    if (ptr) BIO_free(ptr);
}

template<>
void Deleter::operator()(EVP_PKEY* ptr) const
{
    if (ptr) EVP_PKEY_free(ptr);
}

template<>
void Deleter::operator()(X509* ptr) const
{
    if (ptr) X509_free(ptr);
}

template<>
void Deleter::operator()(X509_NAME* ptr) const
{
    if (ptr) X509_NAME_free(ptr);
}

using EVPKey = std::unique_ptr<EVP_PKEY, Deleter>;
using X509NamePtr = std::unique_ptr<X509_NAME, Deleter>;

Result<void> Crypto::AddCertificate(
    SSL_CTX* context,
    X509* cert)
{
    FUSION_ASSERT(context);
    FUSION_ASSERT(cert);

    if (X509_STORE* store = SSL_CTX_get_cert_store(context))
    {
        if (X509_STORE_add_cert(store, cert) == 0)
        {
            const uint32_t code = ERR_get_error();

            if (ERR_GET_LIB(code) != ERR_LIB_X509
                || ERR_GET_REASON(code) != X509_R_CERT_ALREADY_IN_HASH_TABLE)
            {
                return Failure(E_FAILURE)
                    .WithContext("Error adding trusted certificate to SSL context's certificate store");
            }
        }
    }

    return Success;
}

Result<Certificate> Crypto::GenerateCertificate(
    CertificateOptions options)
{
    Certificate cert(X509_new());

    switch (options.version)
    {
    case CertificateVersion::v1:
        X509_set_version(cert.get(), X509_VERSION_1);
        break;
    case CertificateVersion::v2:
        X509_set_version(cert.get(), X509_VERSION_2);
        break;
    case CertificateVersion::v3:
        X509_set_version(cert.get(), X509_VERSION_3);
        break;
    }

    ASN1_INTEGER_set(
        X509_get_serialNumber(cert.get()),
        long(options.serial));

    auto ToTimeT = [](const SystemClock::time_point& time)
    {
        return SystemClock::to_time_t(time);
    };

    X509_gmtime_adj(
        X509_get_notBefore(cert.get()), ToTimeT(options.notBefore));
    X509_gmtime_adj(
        X509_get_notAfter(cert.get()), ToTimeT(options.notAfter));

    X509NamePtr name(X509_NAME_new());
    X509_NAME_add_entry_by_txt(
        name.get(),
        "CN",
        MBSTRING_ASC,
        reinterpret_cast<const uint8_t*>(options.name.c_str()),
        -1,
        -1,
        0);
    X509_set_subject_name(cert.get(), name.get());

    X509NamePtr issuer(X509_NAME_new());
    X509_NAME_add_entry_by_txt(
        issuer.get(),
        "CN",
        MBSTRING_ASC,
        reinterpret_cast<const uint8_t*>(options.issuer.c_str()),
        -1,
        -1,
        0);
    X509_set_issuer_name(cert.get(), issuer.get());

    if (options.signer)
    {
        EVPKey privateKey(EVP_PKEY_new());
        // X509_get_privkey();
        X509_sign(cert.get(), privateKey.get(), EVP_sha256());
    }

    return std::move(cert);
}
}
