/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch300: RS1024 polymod (simple share checksum).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rs1024_polymod(const uint32_t *values, size_t n);
 *     — BIP-173-style BCH polymod over 10-bit symbols (SLIP-0039 RS1024).
 *       values[i] are treated as 10-bit words (caller supplies 0..1023).
 *       Returns the 30-bit residual state after folding n symbols.
 *       Empty input (n == 0) returns the initial state 1.
 *       values may be NULL only when n == 0; otherwise returns 1.
 *   __gj_rs1024_polymod  (alias)
 *   __libcgj_batch300_marker = "libcgj-batch300"
 *
 * RS1024 is a Reed-Solomon-style code over GF(1024): each step shifts the
 * 30-bit checksum state by 10 bits and XORs generator coefficients for
 * each set bit of the top 10 bits of the previous state. A correct
 * SLIP-0039 share yields residual 1 after prefix + data + checksum words.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied. Distinct from batch165 Bech32 (5-bit).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch300_marker[] = "libcgj-batch300";

/*
 * SLIP-0039 / RS1024 generator coefficients (public specification values).
 * Ten 30-bit masks, one per bit of the top 10-bit residue of chk.
 */
static const uint32_t s_b300_gen[10] = {
	0x00e0e040u,
	0x01c1c080u,
	0x03838100u,
	0x07070200u,
	0x0e0e0009u,
	0x1c0c2412u,
	0x38086c24u,
	0x3090fc48u,
	0x21b1f890u,
	0x03f3f120u
};

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rs1024_polymod — fold n 10-bit values into an RS1024 checksum state.
 *
 * values: array of n uint32_t words; only the low 10 bits participate
 *         (same as XOR into a 30-bit state — high bits are not masked).
 * n:      number of values. Zero yields the initial residual 1.
 *
 * Algorithm (BIP-173 polymod shape, width 10 instead of 5):
 *   chk = 1
 *   for each v:
 *     b   = chk >> 20
 *     chk = ((chk & 0xfffff) << 10) ^ v
 *     for i in 0..9:
 *       if (b >> i) & 1: chk ^= GEN[i]
 *   return chk
 */
uint32_t
gj_rs1024_polymod(const uint32_t *values, size_t n)
{
	uint32_t uChk = 1u;
	size_t i;
	unsigned j;

	if (n == 0u || values == NULL) {
		return 1u;
	}

	for (i = 0u; i < n; i++) {
		uint32_t uB = uChk >> 20;

		uChk = ((uChk & 0xfffffu) << 10) ^ values[i];
		for (j = 0u; j < 10u; j++) {
			if ((uB >> j) & 1u) {
				uChk ^= s_b300_gen[j];
			}
		}
	}
	return uChk;
}

uint32_t __gj_rs1024_polymod(const uint32_t *values, size_t n)
    __attribute__((alias("gj_rs1024_polymod")));
