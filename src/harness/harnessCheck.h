/*
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#pragma once
#if HARNESS_CHECK
#include <stdbool.h>
#include <stdint.h>
extern int benchmarkHarnessEncrypt(uint64_t *clen, uint64_t mlen);
extern int benchmarkHarnessDecrypt(uint64_t *mlen, uint64_t clen);
#if DEBUG
extern bool testOfficialKat(uint8_t output[]);
#else
extern bool testOfficialKat();
#endif
extern bool testRoundTrip();
extern bool testTamperDetection();
#endif