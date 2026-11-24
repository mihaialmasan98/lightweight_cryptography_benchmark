/*
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <stdint.h>
#include "defines.h"

#if HARNESS_CHECK
#if defined(USE_ASCON) || defined(USE_XOODYAC)
#define EXPECTED_KAT       16U
#elif defined(USE_TINYJAMBU)
#define EXPECTED_KAT        8U
#endif
extern const uint8_t expected_empty_kat[EXPECTED_KAT];
#endif

extern const uint8_t key[KEY_BYTES];
extern const uint8_t nonce[NONCE_BYTES];
extern const uint8_t ad[AD_BYTES];
extern const uint8_t plaintext[MAX_MESSAGE_BYTES];
extern uint8_t ciphertext[CIPHERTEXT_BYTES];
extern uint8_t recovered[MAX_MESSAGE_BYTES];

/*
 * Prevent the compiler from removing the result of the
 * benchmark operation as unused.
 */
extern volatile uint8_t resultSink;

#if RAM_CHECK
extern uint16_t encPeakRam;
extern uint16_t decPeakRam;
#endif