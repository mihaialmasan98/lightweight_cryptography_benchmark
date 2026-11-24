/*
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#if HARNESS_CHECK
#include <string.h>
#include "harnessCheck.h"
#include "data.h"

#include <crypto_aead.h>
int benchmarkHarnessEncrypt(uint64_t *clen, uint64_t mlen)
{
    return crypto_aead_encrypt(ciphertext, clen, plaintext, mlen, ad, sizeof(ad), NULL, nonce, key);
}
int benchmarkHarnessDecrypt(uint64_t *mlen, uint64_t clen)
{
    return crypto_aead_decrypt(recovered, mlen, NULL, ciphertext, clen, ad, sizeof(ad), nonce, key);
}

#if DEBUG
bool testOfficialKat(uint8_t output[])
#else
bool testOfficialKat()
#endif
{
    uint8_t dummy = 0u;
    uint64_t outputLen = 0u;
    #if DEBUG
    if (NULL == output) { return false; }
    #else
    uint8_t output[TAG_BYTES];
    #endif
    int result = crypto_aead_encrypt(output,
                                     &outputLen,
                                     &dummy,
                                     0u, // plaintext length
                                     &dummy,
                                     0u,   // associated-data length
                                     NULL, // secret nonce unused
                                     nonce,
                                     key);
    if (result != 0)
    {
        return false;
    }

    if (outputLen != EXPECTED_KAT)
    {
        return false;
    }

    return 0 == memcmp(output, expected_empty_kat, EXPECTED_KAT);
}

bool testRoundTrip()
{
    uint64_t ciphertextLen = 0u;
    uint64_t recoveredLen = 0u;

    int encResult = benchmarkHarnessEncrypt(&ciphertextLen, MESSAGE_BYTES);
    if ((encResult != 0) || (ciphertextLen != MESSAGE_BYTES + TAG_BYTES))
    {
        return false;
    }

    int decResult = benchmarkHarnessDecrypt(&recoveredLen, ciphertextLen);
    if ((decResult != 0) || (recoveredLen != MESSAGE_BYTES))
    {
        return false;
    }

    return 0 == memcmp(recovered, plaintext, MESSAGE_BYTES);
}

bool testTamperDetection()
{
    uint64_t ciphertextLen = 0u;
    uint64_t recoveredLen = 0u;

    int encResult = benchmarkHarnessEncrypt(&ciphertextLen, MESSAGE_BYTES);
    if ((encResult != 0) || (ciphertextLen != MESSAGE_BYTES + TAG_BYTES))
    {
        return false;
    }

    /* Change one bit in the authentication tag. */
    ciphertext[ciphertextLen - 1u] ^= 0x01u;

    int decResult = benchmarkHarnessDecrypt(&recoveredLen, ciphertextLen);

    /* Restore buffer. */
    ciphertext[ciphertextLen - 1u] ^= 0x01u;

    // Authentication failure must return non-zero.
    return decResult != 0;
}
#endif