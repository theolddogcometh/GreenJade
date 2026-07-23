/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6047: Deck APU logical-to-core map stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_apu_core_of_logical_6047(uint32_t cpu);
 *     - Map soft logical CPU index to physical core id under the Deck
 *       Van Gogh pairing (core = cpu / 2 for cpu < 8). Returns
 *       UINT32_MAX when cpu is out of range. Soft pure-data only.
 *   uint32_t __gj_deck_apu_core_of_logical_6047  (alias)
 *   __libcgj_batch6047_marker = "libcgj-batch6047"
 *
 * Exclusive continuum CREATE-ONLY (6041-6050: CPU core/thread topology
 * stubs for Deck APU). Unique gj_deck_apu_core_of_logical_6047 surface
 * only; no multi-def. Distinct from validity stub 6046. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6047_marker[] = "libcgj-batch6047";

#define B6047_LOGICAL_CPUS  8u
#define B6047_SMT_PER_CORE  2u
#define B6047_INVALID       0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6047_core_of_logical(uint32_t u32Cpu)
{
	if (u32Cpu >= B6047_LOGICAL_CPUS) {
		return B6047_INVALID;
	}
	return u32Cpu / B6047_SMT_PER_CORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_apu_core_of_logical_6047 - map logical CPU to physical core.
 *
 * cpu: logical CPU index (0-based)
 *
 * Returns physical core id in [0, 3] for valid indices, else
 * UINT32_MAX. Soft Van Gogh SMT pairing (threads 2k,2k+1 share core k).
 * No parent wires.
 */
uint32_t
gj_deck_apu_core_of_logical_6047(uint32_t cpu)
{
	(void)NULL;
	return b6047_core_of_logical(cpu);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_apu_core_of_logical_6047(uint32_t cpu)
    __attribute__((alias("gj_deck_apu_core_of_logical_6047")));
