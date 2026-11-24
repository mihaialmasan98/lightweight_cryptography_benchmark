/*
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#pragma once
#if RAM_CHECK
#include <stdint.h>
extern uint16_t measureEncryptStack(uint64_t *ciphertextLen, uint64_t messageLen);
extern uint16_t measureDecryptStack(uint64_t *recoveredLen, uint64_t ciphertextLen);
#endif