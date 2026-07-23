/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1182: CPUID feature word AVX bit probe (ECX[28]).
 *
 * Surface (unique symbols):
 *   int gj_cpu_has_avx_bits(uint32_t ecx);
 *     — Return 1 if CPUID.1 ECX bit 28 (AVX) is set in the
 *       caller-supplied feature word; else 0. Does not execute CPUID.
 *   int __gj_cpu_has_avx_bits  (alias)
 *   __libcgj_batch1182_marker = "libcgj-batch1182"
 *
 * Distinct from gj_cpu_has_avx2 (batch95, leaf 7 EBX[5]) — pure bit test on pre-fetched words; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1182_marker[] = "libcgj-batch1182";

/* CPUID.1:ECX[28] = AVX */
#define B1182_AVX_BIT  28u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpu_has_avx_bits — 1 if ecx has AVX feature bit set.
 *
 * ecx: CPUID leaf 1 ECX (or equivalent feature word). Pure integer test.
 */
int
gj_cpu_has_avx_bits(uint32_t u32Ecx)
{
	return ((u32Ecx & (1u << B1182_AVX_BIT)) != 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cpu_has_avx_bits(uint32_t u32Ecx)
    __attribute__((alias("gj_cpu_has_avx_bits")));
