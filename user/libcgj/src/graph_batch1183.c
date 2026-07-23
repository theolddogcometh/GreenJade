/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1183: CPUID feature word AES-NI bit probe (ECX[25]).
 *
 * Surface (unique symbols):
 *   int gj_cpu_has_aes_bits(uint32_t ecx);
 *     — Return 1 if CPUID.1 ECX bit 25 (AES-NI) is set in the
 *       caller-supplied feature word; else 0. Does not execute CPUID.
 *   int __gj_cpu_has_aes_bits  (alias)
 *   __libcgj_batch1183_marker = "libcgj-batch1183"
 *
 * Distinct from gj_cpu_has_aesni (batch83, runtime CPUID) — pure bit test on pre-fetched words; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1183_marker[] = "libcgj-batch1183";

/* CPUID.1:ECX[25] = AES-NI */
#define B1183_AES_BIT  25u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpu_has_aes_bits — 1 if ecx has AES-NI feature bit set.
 *
 * ecx: CPUID leaf 1 ECX (or equivalent feature word). Pure integer test.
 */
int
gj_cpu_has_aes_bits(uint32_t u32Ecx)
{
	return ((u32Ecx & (1u << B1183_AES_BIT)) != 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cpu_has_aes_bits(uint32_t u32Ecx)
    __attribute__((alias("gj_cpu_has_aes_bits")));
