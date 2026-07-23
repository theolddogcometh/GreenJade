/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3065: unsigned Q32.32 divide (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_q32_div_u(uint64_t a, uint64_t b);
 *     - Unsigned Q32.32: floor((a * 2^32) / b). Div-by-zero → 0.
 *       Low 64 bits of the mathematical quotient (wrap on overflow).
 *       No __int128: uses (a/b)<<32 + ((a%b)<<32)/b with a 128/64
 *       bit-restoring divider for the remainder term.
 *   uint64_t __gj_q32_div_u  (alias)
 *   __libcgj_batch3065_marker = "libcgj-batch3065"
 *
 * Q32.32 layout (unsigned 64-bit): 1.0 == 0x0000000100000000ull.
 * Exclusive _u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3065_marker[] = "libcgj-batch3065";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Low 64 bits of (n_hi:n_lo) / d for unsigned 128-bit numerator.
 * Bit-restoring division; handles a 65-bit running remainder via a
 * shifted-out high bit flag (no __int128).
 */
static uint64_t
b3065_div128_64(uint64_t u64NHi, uint64_t u64NLo, uint64_t u64D)
{
	uint64_t u64Q;
	uint64_t u64R;
	uint64_t u64Bit;
	unsigned uI;

	u64Q = 0ull;
	u64R = u64NHi;
	for (uI = 0u; uI < 64u; uI++) {
		u64Bit = u64R >> 63;
		u64R = (u64R << 1) | ((u64NLo >> (63u - uI)) & 1ull);
		if (u64Bit != 0ull || u64R >= u64D) {
			u64R -= u64D;
			u64Q |= (1ull << (63u - uI));
		}
	}
	return u64Q;
}

/*
 * floor((a << 32) / b) mod 2^64.
 * Identity: (a<<32)/b = (a/b)<<32 + ((a%b)<<32)/b
 * (mod 2^64 on the first term when a/b is large).
 */
static uint64_t
b3065_div_shl32(uint64_t u64A, uint64_t u64B)
{
	uint64_t u64Q;
	uint64_t u64R;
	uint64_t u64Add;
	uint64_t u64RHi;
	uint64_t u64RLo;

	u64Q = (u64A / u64B) << 32;
	u64R = u64A % u64B;
	/* (r << 32) as 128-bit: hi = r>>32, lo = r<<32. r < b ⇒ hi < b. */
	u64RHi = u64R >> 32;
	u64RLo = u64R << 32;
	u64Add = b3065_div128_64(u64RHi, u64RLo, u64B);
	return u64Q + u64Add;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q32_div_u — divide two unsigned Q32.32 values.
 *
 * Numerator is scaled by 2^32 before division. Division by zero returns
 * 0 (no errno; freestanding-safe). No __int128. No parent wires.
 */
uint64_t
gj_q32_div_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	if (u64B == 0ull) {
		return 0ull;
	}
	return b3065_div_shl32(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_q32_div_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_q32_div_u")));
