/*
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#pragma once
#if TIMING_CHECK
#include <stdint.h>
extern uint16_t measureEncryptTiming(uint64_t *ciphertextLen, uint64_t messageLen);
extern uint16_t measureDecryptTiming(uint64_t *recoveredLen, uint64_t ciphertextLen);
#endif