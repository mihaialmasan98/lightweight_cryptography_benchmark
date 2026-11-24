/*
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#define KEY_BYTES          16u
#if defined(USE_ASCON) || defined(USE_XOODYAC)
#define NONCE_BYTES        16u
#define TAG_BYTES          16u
#elif defined(USE_TINYJAMBU)
#define NONCE_BYTES        12u
#define TAG_BYTES           8u
#endif
#define AD_BYTES           16u
#define MIN_MESSAGE_BYTES   8u
#define MESSAGE_BYTES      16u
#define MAX_MESSAGE_BYTES 128u
#define CIPHERTEXT_BYTES (MAX_MESSAGE_BYTES + TAG_BYTES)

#define BENCHMARK_RUNS     20u
#define TIMER_PRESCALER     8u

#define STACK_PATTERN    0xA5u
#define SRAM_BYTES ((unsigned short)(RAMEND - RAMSTART + 1u))

#ifdef __cplusplus
}
#endif