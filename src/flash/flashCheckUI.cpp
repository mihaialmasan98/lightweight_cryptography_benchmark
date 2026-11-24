/*
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#if FLASH_CHECK
#include <stdint.h>
#include <HardwareSerial.h>
#include "flashCheckUI.hpp"

extern "C"
{
#include "data.h"
#include "flashCheck.h"
}

void checkFlash(void)
{
    uint64_t ciphertextLen = 0u;
    uint64_t recoveredLen = 0u;

    if(!Serial)
    {
        Serial.begin(BAUD);
        while (!Serial) {;}
    }

    Serial.println(F("\n=== Flash measurement harness ==="));
#if USE_ASCON
    Serial.println(F("Build: Ascon-AEAD128 / avr"));
#elif USE_TINYJAMBU
    Serial.println(F("Build: TinyJAMBU_128 / avr5"));
#elif USE_XOODYAC
    Serial.println(F("Build: Xoodyac / avr"));
#else
    Serial.println(F("Build: Harness baseline"));
#endif

    int encResult = benchmarkFlashEncrypt(&ciphertextLen, MESSAGE_BYTES);
    int decResult = benchmarkFlashDecrypt(&recoveredLen, ciphertextLen);
    
    //Make the produced data observable so that the compiler cannot simply remove the operations.
    resultSink = ciphertext[0] ^ recovered[0];

    Serial.print(F("ENC result: "));
    Serial.println(encResult);

    Serial.print(F("DEC result: "));
    Serial.println(decResult);

    Serial.print(F("Ciphertext length: "));
    Serial.println((uint8_t)ciphertextLen);

    Serial.print(F("Recovered length: "));
    Serial.println((uint8_t)recoveredLen);

    Serial.print(F("Result sink: "));
    Serial.println(resultSink);
}
#endif