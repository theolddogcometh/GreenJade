/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6042: Deck APU soft physical core count.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_apu_phys_cores_6042(void);
 *     - Returns the soft compile-time physical core count for the
 *       Steam Deck Van Gogh APU topology stub (always 4 Zen 2 cores).
 *       Not a runtime topology probe.
 *   uint32_t __gj_deck_apu_phys_cores_6042  (alias)
 *   __libcgj_batch6042_marker = "libcgj-batch6042"
 *
 * Exclusive continuum CREATE-ONLY (6041-6050: CPU core/thread topology
 * stubs for Deck APU). Unique gj_deck_apu_phys_cores_6042 surface only;
 * no multi-def. Distinct from gj_deck_apu_logical_cpus_6041. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6042_marker[] = "libcgj-batch6042";

/* Steam Deck Van Gogh: 4 physical Zen 2 cores. */
#define B6042_PHYS_CORES  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6042_phys_cores(void)
{
	return B6042_PHYS_CORES;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_apu_phys_cores_6042 - soft Deck APU physical core count.
 *
 * Always returns 4 (Van Gogh physical cores). Soft pure-data constant;
 * does not call CPUID or read sysfs. No parent wires.
 */
uint32_t
gj_deck_apu_phys_cores_6042(void)
{
	(void)NULL;
	return b6042_phys_cores();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_apu_phys_cores_6042(void)
    __attribute__((alias("gj_deck_apu_phys_cores_6042")));
