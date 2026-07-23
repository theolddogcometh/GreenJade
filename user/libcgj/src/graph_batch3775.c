/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3775: Rabin–Karp rolling-hash slide (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rabin_roll_u(uint32_t hash, uint8_t out_b, uint8_t in_b,
 *                            uint32_t base_pow, uint32_t base,
 *                            uint32_t mod);
 *     - Advance a fixed-width window hash by one byte:
 *         h' = ((h - out_b * base_pow) * base + in_b)  (mod mod)
 *       base_pow should be base^(m-1) mod mod for window width m.
 *       mod==0 uses uint32 natural wrap (no reduction). Distinct name
 *       from gj_rabin_karp_roll (batch1224, unsigned out/in).
 *   uint32_t __gj_rabin_roll_u  (alias)
 *   __libcgj_batch3775_marker = "libcgj-batch3775"
 *
 * Exclusive continuum CREATE-ONLY (3771-3780). Unique gj_rabin_roll_u
 * surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3775_marker[] = "libcgj-batch3775";

/* ---- freestanding helpers ---------------------------------------------- */

/* One modular (or wrap) window slide. */
static uint32_t
b3775_roll(uint32_t uHash, uint8_t u8Out, uint8_t u8In, uint32_t uBasePow,
           uint32_t uBase, uint32_t uMod)
{
	uint64_t u64H;
	uint64_t u64Out;
	uint64_t u64Tmp;

	if (uMod == 0u) {
		u64H = (uint64_t)uHash;
		u64Out = (uint64_t)u8Out * (uint64_t)uBasePow;
		u64H = u64H - u64Out;
		u64H = u64H * (uint64_t)uBase;
		u64H = u64H + (uint64_t)u8In;
		return (uint32_t)u64H;
	}

	u64H = (uint64_t)(uHash % uMod);
	u64Out = ((uint64_t)u8Out * (uint64_t)(uBasePow % uMod)) %
	         (uint64_t)uMod;
	if (u64H >= u64Out) {
		u64Tmp = u64H - u64Out;
	} else {
		u64Tmp = u64H + (uint64_t)uMod - u64Out;
	}
	u64Tmp = (u64Tmp * (uint64_t)(uBase % uMod)) % (uint64_t)uMod;
	u64Tmp = (u64Tmp + (uint64_t)u8In) % (uint64_t)uMod;
	return (uint32_t)u64Tmp;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rabin_roll_u — slide a Rabin–Karp window hash by one byte.
 *
 * hash:     current window hash
 * out_b:    byte leaving the window (left)
 * in_b:     byte entering the window (right)
 * base_pow: base^(m-1) (mod mod)
 * base:     polynomial base
 * mod:      modulus (0 → uint32 wrap)
 *
 * Returns the updated hash. No parent wires.
 */
uint32_t
gj_rabin_roll_u(uint32_t hash, uint8_t out_b, uint8_t in_b,
                uint32_t base_pow, uint32_t base, uint32_t mod)
{
	(void)NULL;
	return b3775_roll(hash, out_b, in_b, base_pow, base, mod);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rabin_roll_u(uint32_t hash, uint8_t out_b, uint8_t in_b,
                           uint32_t base_pow, uint32_t base, uint32_t mod)
    __attribute__((alias("gj_rabin_roll_u")));
