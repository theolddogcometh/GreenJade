/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5402: Deck native ABI probe (flag bits).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_abi_flag_mask_5402(void);
 *     - Returns the soft ABI feature mask (bit0=x64, bit1=SSE2).
 *   uint32_t gj_deck_abi_has_x64_5402(uint32_t flags);
 *     - Return 1 when flags has bit0 set, else 0.
 *   uint32_t gj_deck_abi_has_sse2_5402(uint32_t flags);
 *     - Return 1 when flags has bit1 set, else 0.
 *   uint32_t gj_deck_abi_probe_score_5402(void);
 *     - Returns 5402 (wave-tagged ABI probe score).
 *   uint32_t __gj_deck_abi_flag_mask_5402  (alias)
 *   uint32_t __gj_deck_abi_has_x64_5402  (alias)
 *   uint32_t __gj_deck_abi_has_sse2_5402  (alias)
 *   uint32_t __gj_deck_abi_probe_score_5402  (alias)
 *   __libcgj_batch5402_marker = "libcgj-batch5402"
 *
 * Exclusive continuum CREATE-ONLY (5401-5410). Deck native ABI probe
 * toward bar #3. Unique batch-suffixed symbols only; distinct from
 * gj_deck_native_abi_probe_5401 and gj_proton_abi_ok_u (batch5355).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5402_marker[] = "libcgj-batch5402";

/* Soft ABI feature bits: bit0 = x64, bit1 = SSE2. */
#define B5402_BIT_X64   0x1u
#define B5402_BIT_SSE2  0x2u
#define B5402_MASK      (B5402_BIT_X64 | B5402_BIT_SSE2)
#define B5402_SCORE     5402u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5402_mask(void)
{
	return B5402_MASK;
}

static uint32_t
b5402_has(uint32_t u32Flags, uint32_t u32Bit)
{
	return ((u32Flags & u32Bit) != 0u) ? 1u : 0u;
}

static uint32_t
b5402_score(void)
{
	return B5402_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_abi_flag_mask_5402 - soft Deck ABI feature mask.
 *
 * Always returns 0x3 (x64 | SSE2). Pure product-path constant.
 */
uint32_t
gj_deck_abi_flag_mask_5402(void)
{
	(void)NULL;
	return b5402_mask();
}

/*
 * gj_deck_abi_has_x64_5402 - test bit0 (x64) in an ABI flag word.
 *
 * flags: candidate feature mask
 *
 * Returns 1 if bit0 is set, else 0.
 */
uint32_t
gj_deck_abi_has_x64_5402(uint32_t u32Flags)
{
	return b5402_has(u32Flags, B5402_BIT_X64);
}

/*
 * gj_deck_abi_has_sse2_5402 - test bit1 (SSE2) in an ABI flag word.
 *
 * flags: candidate feature mask
 *
 * Returns 1 if bit1 is set, else 0.
 */
uint32_t
gj_deck_abi_has_sse2_5402(uint32_t u32Flags)
{
	return b5402_has(u32Flags, B5402_BIT_SSE2);
}

/*
 * gj_deck_abi_probe_score_5402 - wave-tagged ABI probe score.
 *
 * Always returns 5402.
 */
uint32_t
gj_deck_abi_probe_score_5402(void)
{
	return b5402_score();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_deck_abi_flag_mask_5402(void)
    __attribute__((alias("gj_deck_abi_flag_mask_5402")));

uint32_t __gj_deck_abi_has_x64_5402(uint32_t u32Flags)
    __attribute__((alias("gj_deck_abi_has_x64_5402")));

uint32_t __gj_deck_abi_has_sse2_5402(uint32_t u32Flags)
    __attribute__((alias("gj_deck_abi_has_sse2_5402")));

uint32_t __gj_deck_abi_probe_score_5402(void)
    __attribute__((alias("gj_deck_abi_probe_score_5402")));
