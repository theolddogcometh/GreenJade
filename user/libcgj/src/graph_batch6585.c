/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6585: CPUID feature register bit test stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cpuid_feat_bit_test_6585(uint32_t reg, uint32_t bit);
 *     - Returns 1 when bit is in [0, 31] and set in reg, else 0.
 *       Soft pure-integer feature-bit probe; not a live CPUID result.
 *   uint32_t __gj_cpuid_feat_bit_test_6585  (alias)
 *   __libcgj_batch6585_marker = "libcgj-batch6585"
 *
 * Exclusive continuum CREATE-ONLY (6581-6590: CPUID feature leaf
 * stubs). Unique gj_cpuid_feat_bit_test_6585 surface only; no multi-def.
 * Soft EAX/EBX/ECX/EDX-style bit lane test. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6585_marker[] = "libcgj-batch6585";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6585_bit_test(uint32_t uReg, uint32_t uBit)
{
	if (uBit > 31u) {
		return 0u;
	}
	return ((uReg >> uBit) & 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpuid_feat_bit_test_6585 - soft feature-register bit probe.
 *
 * reg: candidate EAX/EBX/ECX/EDX feature word.
 * bit: bit index (0..31 valid).
 * Returns 1 if the bit is set and in range, else 0. No parent wires.
 */
uint32_t
gj_cpuid_feat_bit_test_6585(uint32_t uReg, uint32_t uBit)
{
	(void)NULL;
	return b6585_bit_test(uReg, uBit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cpuid_feat_bit_test_6585(uint32_t uReg, uint32_t uBit)
    __attribute__((alias("gj_cpuid_feat_bit_test_6585")));
