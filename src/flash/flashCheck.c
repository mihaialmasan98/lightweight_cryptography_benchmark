/*
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#if FLASH_CHECK
#include <stddef.h>
#include "flashCheck.h"
#include "data.h"

#if USE_ASCON || USE_TINYJAMBU || USE_XOODYAC
#include <crypto_aead.h>
#endif

/*
 * In ASCON, TinyJAMBU and Xoodyac build, the cryptography is done via crypto_aead_encrypt.
 *
 * In HARNESS build, a minimal baseline implementation is given.
 * It deliberately performs no cryptography.
 * It only touches the same input/output interface so that
 * the benchmark infrastructure remains present (not excluded by compiler).
 */
int benchmarkFlashEncrypt(uint64_t *clen, uint64_t mlen)
{
#if USE_ASCON || USE_TINYJAMBU || USE_XOODYAC
    return crypto_aead_encrypt(ciphertext, clen, plaintext, mlen, ad, sizeof(ad), NULL, nonce, key);
#else
    ciphertext[0] = plaintext[0] ^ ad[0] ^ nonce[0] ^ key[0];
    if (NULL != clen) { *clen = mlen + TAG_BYTES; }
    return 0;
#endif
}

/*
 * In ASCON, TinyJAMBU and Xoodyac build, the cryptography is resolved via crypto_aead_decrypt.
 * In USE_TINYJAMBU build, the cryptography is resolved via benchmarkFlashDecrypt.
 *
 * In HARNESS build, a minimal baseline implementation is given.
 * It deliberately performs no cryptography.
 * It only touches the same input/output interface so that
 * the benchmark infrastructure remains present (not excluded by compiler).
 */
int benchmarkFlashDecrypt(uint64_t *mlen, uint64_t clen)
{
#if USE_ASCON || USE_TINYJAMBU || USE_XOODYAC
    return crypto_aead_decrypt(recovered, mlen, NULL, ciphertext, clen, ad, sizeof(ad), nonce, key);
#else
    recovered[0] = ciphertext[0] ^ ad[0] ^ nonce[0] ^ key[0];
    if (NULL != mlen) { *mlen = (clen >= TAG_BYTES)? clen - TAG_BYTES : 0u; }
    return 0;
#endif
}
#endif
