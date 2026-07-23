/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch51: TEA (Tiny Encryption Algorithm) and XTEA
 * 64-bit block ciphers with 128-bit key. Integer/pointer only (no SSE).
 * Clean-room from public algorithm descriptions (Wheeler & Needham).
 *
 * Known values (all-zero key and block, 32 cycles / 32 rounds):
 *   TEA:  plain 00000000 00000000 -> 41ea3a0a 94baa940
 *   XTEA: plain 00000000 00000000 -> dee9d4d8 f7131ed9
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch51_marker[] = "libcgj-batch51";

/* Golden ratio constant (floor(2^32 * (sqrt(5)-1)/2)); shared by TEA/XTEA. */
#define B51_DELTA 0x9E3779B9u

/* TEA fixed cycle count (32 cycles = 64 Feistel half-rounds). */
#define B51_TEA_CYCLES 32u

/* Default XTEA rounds when caller passes 0. */
#define B51_XTEA_DEFAULT_ROUNDS 32u

/* sum after 32 TEA cycles: DELTA * 32 == 0xC6EF3720 */
#define B51_TEA_SUM_FINAL 0xC6EF3720u

/* ========================================================================
 * TEA — Tiny Encryption Algorithm (Wheeler & Needham, 1994)
 * 64-bit block (v[0], v[1]), 128-bit key (key[0..3]), 32 fixed cycles.
 * ======================================================================== */

void
tea_encrypt(const uint32_t key[4], uint32_t v[2])
{
    uint32_t v0, v1, sum, k0, k1, k2, k3;
    unsigned i;

    if (key == NULL || v == NULL) {
        return;
    }

    v0 = v[0];
    v1 = v[1];
    k0 = key[0];
    k1 = key[1];
    k2 = key[2];
    k3 = key[3];
    sum = 0u;

    for (i = 0u; i < B51_TEA_CYCLES; i++) {
        sum += B51_DELTA;
        v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
        v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
    }

    v[0] = v0;
    v[1] = v1;
}

void
tea_decrypt(const uint32_t key[4], uint32_t v[2])
{
    uint32_t v0, v1, sum, k0, k1, k2, k3;
    unsigned i;

    if (key == NULL || v == NULL) {
        return;
    }

    v0 = v[0];
    v1 = v[1];
    k0 = key[0];
    k1 = key[1];
    k2 = key[2];
    k3 = key[3];
    sum = B51_TEA_SUM_FINAL;

    for (i = 0u; i < B51_TEA_CYCLES; i++) {
        v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
        v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
        sum -= B51_DELTA;
    }

    v[0] = v0;
    v[1] = v1;
}

void __tea_encrypt(const uint32_t key[4], uint32_t v[2])
    __attribute__((alias("tea_encrypt")));
void __tea_decrypt(const uint32_t key[4], uint32_t v[2])
    __attribute__((alias("tea_decrypt")));

/* ========================================================================
 * XTEA — eXtended TEA (Needham & Wheeler, 1997)
 * Same block/key size; configurable rounds (default 32 if rounds == 0).
 * ======================================================================== */

void
xtea_encrypt(const uint32_t key[4], uint32_t v[2], unsigned rounds)
{
    uint32_t v0, v1, sum;
    unsigned i;

    if (key == NULL || v == NULL) {
        return;
    }

    if (rounds == 0u) {
        rounds = B51_XTEA_DEFAULT_ROUNDS;
    }

    v0 = v[0];
    v1 = v[1];
    sum = 0u;

    for (i = 0u; i < rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3u]);
        sum += B51_DELTA;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3u]);
    }

    v[0] = v0;
    v[1] = v1;
}

void
xtea_decrypt(const uint32_t key[4], uint32_t v[2], unsigned rounds)
{
    uint32_t v0, v1, sum;
    unsigned i;

    if (key == NULL || v == NULL) {
        return;
    }

    if (rounds == 0u) {
        rounds = B51_XTEA_DEFAULT_ROUNDS;
    }

    v0 = v[0];
    v1 = v[1];
    sum = B51_DELTA * (uint32_t)rounds;

    for (i = 0u; i < rounds; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3u]);
        sum -= B51_DELTA;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3u]);
    }

    v[0] = v0;
    v[1] = v1;
}

void __xtea_encrypt(const uint32_t key[4], uint32_t v[2], unsigned rounds)
    __attribute__((alias("xtea_encrypt")));
void __xtea_decrypt(const uint32_t key[4], uint32_t v[2], unsigned rounds)
    __attribute__((alias("xtea_decrypt")));
