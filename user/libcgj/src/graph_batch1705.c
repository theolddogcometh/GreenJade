/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1705: XXH64 prime constant accessor.
 *
 * Surface (unique symbols):
 *   uint64_t gj_xxh64_prime_a(void);
 *     — Returns XXH64 PRIME64_1 = 0x9E3779B185EBCA87ULL
 *       (public xxHash 64-bit prime #1; used in rounds / merges).
 *   uint64_t __gj_xxh64_prime_a  (alias)
 *   __libcgj_batch1705_marker = "libcgj-batch1705"
 *
 * Constant-only surface. Does NOT redefine gj_xxh64 / XXH64.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1705_marker[] = "libcgj-batch1705";

/*
 * XXH64 PRIME64_1 — first of the five public 64-bit xxHash primes.
 * Documented here as the constant returned by gj_xxh64_prime_a.
 */
#define B1705_XXH64_PRIME64_1 0x9E3779B185EBCA87ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1705_prime_a(void)
{
	return B1705_XXH64_PRIME64_1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xxh64_prime_a — return XXH64 PRIME64_1.
 *
 * Always returns 0x9E3779B185EBCA87ULL (xxHash public algorithm constant
 * PRIME64_1). Callers may use this to seed or verify XXH64-related
 * arithmetic without hard-coding the literal at every site.
 */
uint64_t
gj_xxh64_prime_a(void)
{
	return b1705_prime_a();
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_xxh64_prime_a(void)
    __attribute__((alias("gj_xxh64_prime_a")));
