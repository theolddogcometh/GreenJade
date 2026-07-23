/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6048: Deck APU SMT sibling map stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_apu_smt_sibling_6048(uint32_t cpu);
 *     - Return the soft SMT sibling logical CPU for cpu under the Deck
 *       Van Gogh pairing (flip low bit: 0↔1, 2↔3, …). Returns
 *       UINT32_MAX when cpu is out of range. Soft pure-data only.
 *   uint32_t __gj_deck_apu_smt_sibling_6048  (alias)
 *   __libcgj_batch6048_marker = "libcgj-batch6048"
 *
 * Exclusive continuum CREATE-ONLY (6041-6050: CPU core/thread topology
 * stubs for Deck APU). Unique gj_deck_apu_smt_sibling_6048 surface
 * only; no multi-def. Distinct from core map 6047. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6048_marker[] = "libcgj-batch6048";

#define B6048_LOGICAL_CPUS  8u
#define B6048_INVALID       0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6048_smt_sibling(uint32_t u32Cpu)
{
	if (u32Cpu >= B6048_LOGICAL_CPUS) {
		return B6048_INVALID;
	}
	/* Adjacent even/odd pair share a physical core. */
	return u32Cpu ^ 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_apu_smt_sibling_6048 - soft Deck APU SMT sibling of cpu.
 *
 * cpu: logical CPU index (0-based)
 *
 * Returns the sibling logical CPU in the same physical core for valid
 * indices, else UINT32_MAX. Soft Van Gogh pairing. No parent wires.
 */
uint32_t
gj_deck_apu_smt_sibling_6048(uint32_t cpu)
{
	(void)NULL;
	return b6048_smt_sibling(cpu);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_apu_smt_sibling_6048(uint32_t cpu)
    __attribute__((alias("gj_deck_apu_smt_sibling_6048")));
