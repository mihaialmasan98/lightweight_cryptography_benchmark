/*
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#if HARNESS_CHECK
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <HardwareSerial.h>
#include "harnessCheckUI.hpp"

extern "C"
{
#include "harnessCheck.h"
#include "data.h"
}

static void printHex(const uint8_t data[], uint8_t len)
{
    if (NULL == data) { return; }
    for (uint8_t i = 0u; i < len; ++i)
    {
        if (data[i] < 0x10u)
        { 
            Serial.print('0');
        }
        Serial.print(data[i], HEX);
    }
    Serial.println();
}

void checkHarness(void)
{
    if (!Serial)
    {
        Serial.begin(BAUD);
        while (!Serial) {;}
    }
#if USE_ASCON
    Serial.println(F("\n=== Ascon-AEAD128 / Arduino Uno ==="));
    Serial.println(F("NIST SP 800-232"));
    Serial.println(F("Implementation: ascon-c / avr\n"));
#elif USE_TINYJAMBU
    Serial.println(F("\n=== TinyJAMBU_128 / Arduino Uno ==="));
    Serial.println(F("Implementation: avr5\n"));
#elif USE_XOODYAC
    Serial.println(F("\n=== Xoodyac / Arduino Uno ==="));
    Serial.println(F("Implementation: avr\n"));
#endif

#if DEBUG
    uint8_t output[TAG_BYTES];
    bool katOk = testOfficialKat(output);
#else
    bool katOk = testOfficialKat();
#endif
    Serial.print(F("Official KAT: "));
    Serial.println(katOk ? F("PASS") : F("FAIL"));
#if DEBUG
    Serial.print(F("KAT: "));
    printHex(output, TAG_BYTES);
    Serial.println();
#endif

    bool roundTripOk = testRoundTrip();
    Serial.print(F("A=16, M=16 round-trip: "));
    Serial.println(roundTripOk ? F("PASS") : F("FAIL"));

    bool tamperOk = testTamperDetection();
    Serial.print(F("Tamper/tag detection: "));
    Serial.println(tamperOk ? F("PASS") : F("FAIL"));

    uint64_t ciphertextLen = 0u;
    uint64_t plaintextLen = 0u;

    if (0 == benchmarkHarnessEncrypt(&ciphertextLen, MESSAGE_BYTES))
    {
        if (((MESSAGE_BYTES + TAG_BYTES) == ciphertextLen) || (CIPHERTEXT_BYTES >= ciphertextLen)) {
            Serial.print(F("Ciphertext length: "));
            Serial.println((uint8_t)ciphertextLen);

            Serial.print(F("Ciphertext: "));
            printHex(ciphertext, (uint8_t)ciphertextLen);
        } else { Serial.print(F("Ciphertext length larger then expected!")); }
    } else { Serial.print(F("Encryption failure!")); }
    if (0 == benchmarkHarnessDecrypt(&plaintextLen, ciphertextLen))
    {
        if ((MESSAGE_BYTES == plaintextLen) || (MAX_MESSAGE_BYTES >= plaintextLen)) {
            Serial.print(F("Plaintext length: "));
            Serial.println((uint8_t)plaintextLen);

            Serial.print(F("Plaintext: "));
            printHex(recovered, (uint8_t)plaintextLen);
        } else { Serial.print(F("Plaintext length larger then expected!")); }
    } else { Serial.print(F("Decryption failure!")); }
}
#endif
