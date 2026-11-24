/*
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#if TIMING_CHECK
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <crypto_aead.h>
#include "timingCheck.h"
#include "timingInit.h"
#include "data.h"

void timer1Init(void)
{
    register uint8_t oldSreg = SREG; // interrupt state
    cli();                           // noInterrupts();
    {
        TCCR1A = 0u;       // cfg timer1
        TCCR1B = 0u;       // stop timer1
        TCNT1 = 0u;        // clear timer1
        TIFR1 = _BV(TOV1); // clear timer1 overflow flag
    }
    SREG = oldSreg; // restore previous interrupt state
}

uint16_t measureEncryptTiming(uint64_t *ciphertextLen, uint64_t messageLen)
{
    register uint8_t oldSreg = SREG; // interrupt state
    register uint16_t ticks = 0u;
    if (NULL == ciphertextLen) { return ticks; }
    // Avoid Timer0 / UART / other interrupt activity inside the measured interval.
    cli(); // noInterrupts();
    {
        TCNT1 = 0u;        // clear timer1
        TIFR1 = _BV(TOV1); // clear timer1 overflow flag
        /*
         * Timer1 Clock Select bits:
         *
         * CS12:CS10 = 0b010 => '8' prscaler
         *
         * CS12 = 0
         * CS11 = 1
         * CS10 = 0
         *
         * Timer1 clock = F_CPU / 8.
         */
        TCCR1B = _BV(CS11); // start clock with '8' prescaler
        (void)crypto_aead_encrypt(ciphertext,
                                  ciphertextLen,
                                  plaintext,
                                  messageLen,
                                  ad,
                                  sizeof(ad),
                                  NULL,
                                  nonce,
                                  key);
        /*
         * Stop Timer1 before reading it.
         */
        TCCR1B = 0u;   // stop timer1
        ticks = TCNT1; // timer1 value
    }
    SREG = oldSreg; // restore previous interrupt state
    return ticks;
}

uint16_t measureDecryptTiming(uint64_t *recoveredLen, uint64_t ciphertextLen)
{
    register uint8_t oldSreg = SREG; // interrupt state
    register uint16_t ticks = 0u;
    if (NULL == recoveredLen) { return ticks; }
    cli(); // noInterrupts();
    {
        TCNT1 = 0u;         // clear timer1
        TIFR1 = _BV(TOV1);  // clear timer1 overflow flag
        TCCR1B = _BV(CS11); // start clock with '8' prescaler
        (void)crypto_aead_decrypt(recovered,
                                  recoveredLen,
                                  NULL,
                                  ciphertext,
                                  ciphertextLen,
                                  ad,
                                  sizeof(ad),
                                  nonce,
                                  key);
        TCCR1B = 0u;   // stop timer1
        ticks = TCNT1; // timer1 value
    }
    SREG = oldSreg; // restore previous interrupt state
    return ticks;
}
#endif
