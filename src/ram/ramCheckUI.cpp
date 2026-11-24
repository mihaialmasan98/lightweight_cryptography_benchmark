/*
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#if RAM_CHECK
#include <stdint.h>
#include <HardwareSerial.h>
#include "ramCheckUI.hpp"

extern "C"
{
#include "data.h"
#include "ramCheck.h"
}

static void runBenchmark(uint8_t run, uint16_t messageLen)
{
    uint16_t encStack = 0u;
    uint16_t decStack = 0u;
    uint64_t ciphertextLen = 0u;
    uint64_t recoveredLen = 0u;
    /*
     * Measure encryption stack growth.
     *
     * The ciphertext generated here is retained and used
     * directly by the subsequent decryption measurement.
     */
    encStack = measureEncryptStack(&ciphertextLen, messageLen);

    /*
     * Measure decryption stack growth.
     *
     * The SRAM watermark is repainted inside the function,
     * so the previous ENC measurement does not affect the
     * DEC watermark.
     */
    decStack = measureDecryptStack(&recoveredLen, ciphertextLen);

    /*
     * Make the produced data observable so that the compiler
     * cannot simply remove the operations.
     */
    resultSink = ciphertext[0] ^ recovered[0] ^ (uint8_t)ciphertextLen ^ (uint8_t)recoveredLen;

    Serial.print(F("\nM = "));
    Serial.print(messageLen);
    Serial.print(F(" bytes, Run nr. "));
    Serial.println(run);

    Serial.print(F("ENC stack growth: "));
    Serial.print(encStack);
    Serial.print(F(" bytes. Peak RAM usage ENC: "));
    Serial.print(encPeakRam);

    Serial.print(F(" bytes.\nDEC stack growth: "));
    Serial.print(decStack);
    Serial.print(F(" bytes. Peak RAM usage DEC: "));
    Serial.print(decPeakRam);

    Serial.print(F(" bytes.\nCiphertext length: "));
    Serial.print((uint8_t)ciphertextLen);
    Serial.print(F(". Recovered length: "));
    Serial.println((uint8_t)recoveredLen);
}

void checkRAM(void)
{
    if(!Serial)
    {
        Serial.begin(BAUD);
        while (!Serial) {;}
    }
    Serial.println(F("\n=== RAM measurement harness ==="));

#if USE_ASCON
    Serial.println(F("Build: Ascon-AEAD128 / avr"));
#elif USE_TINYJAMBU
    Serial.println(F("Build: TinyJAMBU_128 / avr5"));
#elif USE_XOODYAC
    Serial.println(F("Build: Xoodyac / avr"));
#else
    Serial.println(F("Build: Harness baseline"));
#endif

    Serial.println(F("Arduino Uno / ATmega328P"));
    Serial.println(F("A = 16 bytes"));
    Serial.println(F("M = 8, 16, 32, 64, 128 bytes"));
    Serial.println(F("Runs per message size: 20\n"));
    
    for (uint16_t messageLen = MIN_MESSAGE_BYTES; messageLen <= MAX_MESSAGE_BYTES; messageLen <<= 1u)
    {
        for (uint8_t run = 1u; run <= BENCHMARK_RUNS; ++run)
        {
            runBenchmark(run, messageLen);
        }
    }  
    
    Serial.println(F("\n=== Benchmark complete ==="));
    Serial.print(F("Result sink: "));
    Serial.println(resultSink);
}
#endif