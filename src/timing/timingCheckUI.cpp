/*
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#if TIMING_CHECK
#include <stdint.h>
#include <HardwareSerial.h>
#include "timingCheckUI.hpp"

extern "C"
{
#include "data.h"
#include "timingCheck.h"
}

static void runBenchmark(uint8_t run, uint16_t messageLen)
{
    uint64_t ciphertextLen = 0u;
    uint64_t recoveredLen = 0u;
    /*
     * Measure encryption.
     */
    uint16_t encTicks = measureEncryptTiming(&ciphertextLen, messageLen);

    /*
     * Measure decryption using the ciphertext produced above.
     */
    uint16_t decTicks = measureDecryptTiming(&recoveredLen, ciphertextLen);

    resultSink = ciphertext[0] ^ recovered[0];

    uint32_t encCycles = (uint32_t)encTicks * TIMER_PRESCALER;
    uint32_t decCycles = (uint32_t)decTicks * TIMER_PRESCALER;

    Serial.print(F("\nM = "));
    Serial.print(messageLen);
    Serial.print(F(" B, Run nr. "));
    Serial.println(run);
    
    /*
     * At 16 MHz:
     *
     * 16 cycles = 1 μs
     */
    //float encUs = (float)encCycles / 16.0f;
    //float decUs = (float)decCycles / 16.0f;

    Serial.print(F("ENC ticks: "));
    Serial.println(encTicks);

    Serial.print(F("ENC cycles: "));
    Serial.println(encCycles);

    //Serial.print(F("ENC μs: "));
    //Serial.println(encUs, 2);

    Serial.println();

    Serial.print(F("DEC ticks: "));
    Serial.println(decTicks);

    Serial.print(F("DEC cycles: "));
    Serial.println(decCycles);

    //Serial.print(F("DEC μs: "));
    //Serial.println(decUs, 2);

    Serial.println();
}

void checkTiming(void)
{
    if (!Serial)
    {
        Serial.begin(BAUD);
        while(!Serial) {;}
    }

    Serial.println(F("Arduino Uno / ATmega328P @ 16 MHz"));
#if defined(USE_ASCON)
    Serial.println(F("\n=== Ascon-AEAD128 timing benchmark ==="));
    Serial.println(F("Implementation: ascon-c / avr"));
#elif defined(USE_TINYJAMBU)
    Serial.println(F("\n=== TinyJAMBU timing benchmark ==="));
    Serial.println(F("Implementation: TinyJAMBU_128 / avr5"));
#elif defined(USE_XOODYAC)
    Serial.println(F("\n=== Xoodyac timing benchmark ==="));
    Serial.println(F("Implementation: Xoodyac / avr"));
#endif
    Serial.println(F("A = 16 bytes"));
    Serial.println(F("M = 8, 16, 32, 64, 128 bytes"));
    Serial.println(F("Timer1 prescaler: 8"));
    Serial.println(F("Runs per message size: 20\n"));

    for (uint16_t messageLen = MIN_MESSAGE_BYTES; messageLen <= MAX_MESSAGE_BYTES; messageLen <<= 1u)
    {
        for (uint8_t run = 1u; run <= BENCHMARK_RUNS; ++run)
        {
            runBenchmark(run, messageLen);
        }
    }

    Serial.println(F("=== Benchmark complete ==="));
    Serial.print(F("Result sink: "));
    Serial.println(resultSink);
    Serial.println();
}
#endif