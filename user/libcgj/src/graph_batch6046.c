/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6046: Deck APU logical CPU index validity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_apu_logical_valid_6046(uint32_t cpu);
 *     - Return 1 if cpu is a valid soft logical CPU index for the
 *       Steam Deck Van Gogh topology stub (cpu < 8), else 0. Soft
 *       pure-data range check; not a runtime affinity probe.
 *   uint32_t __gj_deck_apu_logical_valid_6046  (alias)
 *   __libcgj_batch6046_marker = "libcgj-batch6046"
 *
 * Exclusive continuum CREATE-ONLY (6041-6050: CPU core/thread topology
 * stubs for Deck APU). Unique gj_deck_apu_logical_valid_6046 surface
 * only; no multi-def. Distinct from gj_deck_apu_logical_cpus_6041.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6046_marker[] = "libcgj-batch6046";

/* Soft logical CPU count (matches batch6041 stub constant). */
#define B6046_LOGICAL_CPUS  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6046_logical_valid(uint32_t u32Cpu)
{
	return (u32Cpu < B6046_LOGICAL_CPUS) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_apu_logical_valid_6046 - soft Deck logical CPU index check.
 *
 * cpu: candidate logical CPU index (0-based)
 *
 * Returns 1 if cpu is in [0, 8), else 0. Soft pure-data range check
 * against the Van Gogh 8-thread stub. No parent wires.
 */
uint32_t
gj_deck_apu_logical_valid_6046(uint32_t cpu)
{
	(void)NULL;
	return b6046_logical_valid(cpu);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_apu_logical_valid_6046(uint32_t cpu)
    __attribute__((alias("gj_deck_apu_logical_valid_6046")));
