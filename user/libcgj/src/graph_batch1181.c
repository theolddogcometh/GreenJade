/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1181: CPUID feature word SSE2 bit probe (EDX[26]).
 *
 * Surface (unique symbols):
 *   int gj_cpu_has_sse2_bits(uint32_t edx);
 *     — Return 1 if CPUID.1 EDX bit 26 (SSE2) is set in the
 *       caller-supplied feature word; else 0. Does not execute CPUID.
 *   int __gj_cpu_has_sse2_bits  (alias)
 *   __libcgj_batch1181_marker = "libcgj-batch1181"
 *
 * Distinct from runtime CPUID helpers (gj_cpu_has_aesni / avx2) — pure bit test on pre-fetched words; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1181_marker[] = "libcgj-batch1181";

/* CPUID.1:EDX[26] = SSE2 */
#define B1181_SSE2_BIT  26u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpu_has_sse2_bits — 1 if edx has SSE2 feature bit set.
 *
 * edx: CPUID leaf 1 EDX (or equivalent feature word). Pure integer test.
 */
int
gj_cpu_has_sse2_bits(uint32_t u32Edx)
{
	return ((u32Edx & (1u << B1181_SSE2_BIT)) != 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cpu_has_sse2_bits(uint32_t u32Edx)
    __attribute__((alias("gj_cpu_has_sse2_bits")));
