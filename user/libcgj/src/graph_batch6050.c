/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6050: Deck APU topology consistency check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_apu_topo_consistent_6050(uint32_t cores,
 *                                             uint32_t smt,
 *                                             uint32_t logical);
 *     - Return 1 when cores * smt == logical with all factors nonzero
 *       and matching the soft Deck Van Gogh envelope (cores <= 4,
 *       smt <= 2, logical <= 8). Else 0. Soft pure-data product check;
 *       does not call other batch symbols (no parent wires).
 *   uint32_t __gj_deck_apu_topo_consistent_6050  (alias)
 *   __libcgj_batch6050_marker = "libcgj-batch6050"
 *
 * Exclusive continuum CREATE-ONLY (6041-6050: CPU core/thread topology
 * stubs for Deck APU). Unique gj_deck_apu_topo_consistent_6050 surface
 * only; no multi-def. Distinct from count stubs 6041-6043. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6050_marker[] = "libcgj-batch6050";

/* Soft Deck Van Gogh envelope maxima. */
#define B6050_MAX_CORES    4u
#define B6050_MAX_SMT      2u
#define B6050_MAX_LOGICAL  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6050_topo_consistent(uint32_t u32Cores, uint32_t u32Smt, uint32_t u32Logical)
{
	uint32_t u32Prod;

	if (u32Cores == 0u || u32Smt == 0u || u32Logical == 0u) {
		return 0u;
	}
	if (u32Cores > B6050_MAX_CORES
	    || u32Smt > B6050_MAX_SMT
	    || u32Logical > B6050_MAX_LOGICAL) {
		return 0u;
	}
	/* cores and smt small: product cannot overflow uint32. */
	u32Prod = u32Cores * u32Smt;
	if (u32Prod != u32Logical) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_apu_topo_consistent_6050 - soft Deck APU topology consistency.
 *
 * cores:   physical core count claim
 * smt:     threads-per-core claim
 * logical: logical CPU count claim
 *
 * Returns 1 when cores*smt==logical within the Van Gogh envelope
 * (cores<=4, smt<=2, logical<=8, all nonzero), else 0. Soft pure-data
 * only; does not call gj_deck_apu_*_604x symbols. No parent wires.
 */
uint32_t
gj_deck_apu_topo_consistent_6050(uint32_t cores, uint32_t smt,
    uint32_t logical)
{
	(void)NULL;
	return b6050_topo_consistent(cores, smt, logical);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_apu_topo_consistent_6050(uint32_t cores, uint32_t smt,
    uint32_t logical)
    __attribute__((alias("gj_deck_apu_topo_consistent_6050")));
