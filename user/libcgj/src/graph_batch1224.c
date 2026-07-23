/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1224: Rabin–Karp modular rolling-hash step.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rabin_karp_roll(uint32_t hash, unsigned out_byte,
 *                               unsigned in_byte, uint32_t base_pow,
 *                               uint32_t base, uint32_t mod);
 *     — Advance a window hash: remove out_byte (weighted by base_pow =
 *       base^(m-1) mod mod), multiply by base, add in_byte, all mod mod.
 *       If mod == 0, uses 2^32 natural wrap (no reduction). out_byte /
 *       in_byte are taken as low 8 bits only.
 *   uint32_t __gj_rabin_karp_roll  (alias)
 *   __libcgj_batch1224_marker = "libcgj-batch1224"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1224_marker[] = "libcgj-batch1224";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rabin_karp_roll — one modular window slide for Rabin–Karp.
 *
 * hash' = ((hash - out * base_pow) * base + in) mod mod
 * Intermediate values use uint64_t to reduce overflow before mod.
 */
uint32_t
gj_rabin_karp_roll(uint32_t uHash, unsigned uOut, unsigned uIn,
    uint32_t uBasePow, uint32_t uBase, uint32_t uMod)
{
	uint64_t u64H;
	uint64_t u64Out;
	uint64_t u64Tmp;
	uint8_t u8Out;
	uint8_t u8In;

	u8Out = (uint8_t)(uOut & 0xffu);
	u8In = (uint8_t)(uIn & 0xffu);

	if (uMod == 0u) {
		/* Pure 32-bit wrap: (hash - out*base_pow)*base + in */
		u64H = (uint64_t)uHash;
		u64Out = (uint64_t)u8Out * (uint64_t)uBasePow;
		u64H = u64H - u64Out;
		u64H = u64H * (uint64_t)uBase;
		u64H = u64H + (uint64_t)u8In;
		return (uint32_t)u64H;
	}

	u64H = (uint64_t)(uHash % uMod);
	u64Out = ((uint64_t)u8Out * (uint64_t)(uBasePow % uMod)) % (uint64_t)uMod;
	if (u64H >= u64Out) {
		u64Tmp = u64H - u64Out;
	} else {
		u64Tmp = u64H + (uint64_t)uMod - u64Out;
	}
	u64Tmp = (u64Tmp * (uint64_t)(uBase % uMod)) % (uint64_t)uMod;
	u64Tmp = (u64Tmp + (uint64_t)u8In) % (uint64_t)uMod;
	return (uint32_t)u64Tmp;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rabin_karp_roll(uint32_t uHash, unsigned uOut, unsigned uIn,
    uint32_t uBasePow, uint32_t uBase, uint32_t uMod)
    __attribute__((alias("gj_rabin_karp_roll")));
