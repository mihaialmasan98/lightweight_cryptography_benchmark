/*
 * AEAD interface adapter for the lightweight cryptography benchmark.
 *
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#include "crypto_aead.h"
#include "xoodyak-aead.h"
#include <stdint.h>

int crypto_aead_encrypt(unsigned char *c, unsigned long long *clen,
                        const unsigned char *m, unsigned long long mlen,
                        const unsigned char *ad, unsigned long long adlen,
                        const unsigned char *nsec, const unsigned char *npub,
                        const unsigned char *k)
{
    (void)nsec;
    int result = -1;
    if ((NULL != clen) && ((SIZE_MAX - XOODYAK_TAG_SIZE) >= mlen) && (SIZE_MAX >= adlen)) {
        size_t ciphertextLength = 0;
        result = xoodyak_aead_encrypt(c, &ciphertextLength, m, (size_t)mlen, ad, (size_t)adlen, npub, k);
        *clen = ciphertextLength; 
    }
    return result;
}

int crypto_aead_decrypt(unsigned char *m, unsigned long long *mlen,
                        unsigned char *nsec, const unsigned char *c,
                        unsigned long long clen, const unsigned char *ad,
                        unsigned long long adlen, const unsigned char *npub,
                        const unsigned char *k)
{
    (void)nsec;
    int result = -1;
    if ((NULL != mlen) && (XOODYAK_TAG_SIZE <= clen) && (SIZE_MAX >= clen) && (SIZE_MAX >= adlen))
    {
        size_t messageLength = 0;
        result = xoodyak_aead_decrypt(m, &messageLength, c, (size_t)clen, ad, (size_t)adlen, npub, k);
        *mlen = messageLength;
    }
    return result;
}
