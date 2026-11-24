/*
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#include <Arduino.h>
#include <HardwareSerial.h>

#if HARNESS_CHECK
#include "harness/harnessCheckUI.hpp"
#endif
#if FLASH_CHECK
#include "flash/flashCheckUI.hpp"
#endif
#if TIMING_CHECK
extern "C"
{
#include "timing/timingInit.h"
}
#include "timing/timingCheckUI.hpp"
#endif
#if RAM_CHECK
#include "ram/ramCheckUI.hpp"
#endif

#if USE_ASCON || USE_TINYJAMBU || USE_XOODYAC
#include "defines.h"
#if USE_ASCON
extern "C"
{
#include "api.h"
}
static_assert(CRYPTO_KEYBYTES == KEY_BYTES, "Ascon-AEAD128 key size must be 16 bytes.");
static_assert(CRYPTO_NPUBBYTES == NONCE_BYTES, "Ascon-AEAD128 nonce must be 16 bytes.");
static_assert(CRYPTO_ABYTES == TAG_BYTES, "Ascon-AEAD128 authentication tag must be 16 bytes.");
#elif USE_TINYJAMBU
#include <TinyJAMBU.h>
static_assert(TINYJAMBU_128_KEY_SIZE == KEY_BYTES, "TinyJAMBU_128 key size must be 16 bytes.");
static_assert(TINYJAMBU_NONCE_SIZE == NONCE_BYTES, "TinyJAMBU_128 nonce must be 12 bytes.");
static_assert(TINYJAMBU_TAG_SIZE == TAG_BYTES, "TinyJAMBU_128 authentication tag must be 8 bytes.");
#elif USE_XOODYAC
#include <xoodyak-aead.h>
static_assert(XOODYAK_KEY_SIZE == KEY_BYTES, "Xoodyac key size must be 16 bytes.");
static_assert(XOODYAK_NONCE_SIZE == NONCE_BYTES, "Xoodyac nonce must be 16 bytes.");
static_assert(XOODYAK_TAG_SIZE == TAG_BYTES, "Xoodyac authentication tag must be 16 bytes.");
#endif
#endif

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(BAUD);
    delay(1000u);
#if HARNESS_CHECK
    checkHarness();
#endif
#if FLASH_CHECK
    checkFlash();
#endif
#if TIMING_CHECK
    timer1Init();
    checkTiming();
#endif
#if RAM_CHECK
    checkRAM();
#endif
}

void loop()
{
    // put your main code here, to run repeatedly:
    /* Nothing to do. */
}