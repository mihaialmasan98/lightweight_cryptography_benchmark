/*
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#pragma once
#if FLASH_CHECK
#include <stdint.h>
extern int benchmarkFlashEncrypt(uint64_t *clen, uint64_t mlen);
extern int benchmarkFlashDecrypt(uint64_t *mlen, uint64_t clen);
#endif