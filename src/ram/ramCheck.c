/*
 * Copyright (c) 2026 Mihai
 * SPDX-License-Identifier: MIT
 */
#if RAM_CHECK
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ramCheck.h"
#include "data.h"

#if USE_ASCON || USE_TINYJAMBU || USE_XOODYAC
#include <crypto_aead.h>
#endif

/*
 * AVR heap symbols.
 *
 * If malloc() has never been used, __brkval is NULL and
 * __heap_start represents the lower boundary of free SRAM.
 */
extern char __heap_start;
extern void *__brkval;

/*
 * Return the current lower boundary of unused SRAM.
 *
 * No dynamic allocation is expected in this benchmark.
 * Nevertheless, __brkval is checked so that the function
 * remains valid if the heap has been used.
 */
static inline uintptr_t getHeapEnd(void)
{
    return (NULL == __brkval)? (uintptr_t)&__heap_start : (uintptr_t)__brkval;
}

uint16_t measureEncryptStack(uint64_t *ciphertextLen, uint64_t messageLen)
{
    volatile uint8_t* ptr;
    uint16_t stackGrowth = 0u;
    uint16_t baselineSp;
    uintptr_t heapEnd;
    if (NULL == ciphertextLen) { return stackGrowth; }
    /*
     * Save and disable interrupts so that an ISR cannot
     * modify the painted SRAM during the measurement.
     */
    register uint8_t oldSreg = SREG;
    cli();
    {
        /*
         * Determine the currently unused SRAM interval.
        */
        heapEnd = getHeapEnd();
        
        /*
         * Read the current AVR stack pointer.
         */
        baselineSp = SP;
        
        /*
         * Normally SRAM is followed by stack.
         * If it is not, then restore the interrupt state and exit function (0).
        */
        if ((uint16_t)heapEnd >= baselineSp)
        {
            encPeakRam = 0u;
            SREG = oldSreg;
            return stackGrowth; // 0
        }

        /*
         * Paint all the way to the active stack.
         */
        for (ptr = (volatile uint8_t*)heapEnd; (uint16_t)ptr < baselineSp; ++ptr)
        {
            *ptr = STACK_PATTERN;
        }

        /*
         * Execute the operation being measured.
         */
#if USE_ASCON || USE_TINYJAMBU || USE_XOODYAC
        (void)crypto_aead_encrypt(ciphertext,
                                  ciphertextLen,
                                  plaintext,
                                  messageLen,
                                  ad,
                                  sizeof(ad),
                                  NULL,
                                  nonce,
                                  key);
#else
        /*
         * Minimal HARNESS operation.
         */
        ciphertext[0] = plaintext[0] ^ ad[0] ^ nonce[0] ^ key[0];
        *ciphertextLen = messageLen + TAG_BYTES;
#endif

        /*
         * Scan the painted area from low SRAM toward the stack.
         *
         * The first modified byte marks the deepest point reached
         * by the stack inside the painted region.
         */
        ptr = (volatile uint8_t *)heapEnd;
        while (((uint16_t)ptr < baselineSp) && (*ptr == STACK_PATTERN))
        {
            ++ptr;
        }

        /*
         * Then, calculate the difference between the stack and the found point.
        */
        stackGrowth = baselineSp - (uint16_t)ptr;
        encPeakRam = SRAM_BYTES - ((uint16_t)ptr - (uint16_t)heapEnd);
    }
    /*
     * Restore the interrupt state that existed before entering the measurement.
     */
    SREG = oldSreg;

    /*
     * Keep the encryption output observable.
     *
     * This operation occurs outside the measured interval.
     */
    resultSink ^= ciphertext[0];

    return stackGrowth;
}

uint16_t measureDecryptStack(uint64_t *recoveredLen, uint64_t ciphertextLen)
{
    volatile uint8_t* ptr;
    uint16_t stackGrowth = 0u;
    uint16_t baselineSp;
    uintptr_t heapEnd;
    if (NULL == recoveredLen) { return stackGrowth; }
    // Save state and disable interrupts
    register uint8_t oldSreg = SREG;
    cli();
    {
        heapEnd = getHeapEnd();  
        baselineSp = SP;
        // Normally (heapEnd < baselineSp). Else restore the interrupt state and exit function (0).
        if ((uint16_t)heapEnd >= baselineSp)
        {
            decPeakRam = 0u;
            SREG = oldSreg;
            return stackGrowth;
        }
        // Repaint SRAM so that the previous encryption measurement cannot influence the DEC result.
        for (ptr = (volatile uint8_t*)heapEnd; (uint16_t)ptr < baselineSp; ++ptr)
        {
            *ptr = STACK_PATTERN;
        }
        // Execute the operation being measured.
#if USE_ASCON || USE_TINYJAMBU || USE_XOODYAC
        (void)crypto_aead_decrypt(recovered,
                                  recoveredLen,
                                  NULL,
                                  ciphertext,
                                  ciphertextLen,
                                  ad,
                                  sizeof(ad),
                                  nonce,
                                  key);
#else
        //Minimal HARNESS operation.
        recovered[0] = ciphertext[0] ^ ad[0] ^ nonce[0] ^ key[0];
        *recoveredLen = (ciphertextLen >= TAG_BYTES) ? ciphertextLen - TAG_BYTES : 0u;
#endif
        // Scan the painted area between the SRAM and the stack to find the point where the pattern changes.
        ptr = (volatile uint8_t *)heapEnd;
        while (((uint16_t)ptr < baselineSp) && (*ptr == STACK_PATTERN))
        {
            ++ptr;
        }
        // Then, calculate the difference between the stack and the found point.
        stackGrowth = baselineSp - (uint16_t)ptr;
        decPeakRam = SRAM_BYTES - ((uint16_t)ptr - (uint16_t)heapEnd);
    }
    // Restore the interrupt state that existed before entering the measurement.
    SREG = oldSreg;
    resultSink ^= recovered[0] ^ (uint8_t)(*recoveredLen);
    return stackGrowth;
}
#endif
