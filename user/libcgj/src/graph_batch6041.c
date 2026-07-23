/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6041: Deck APU soft logical CPU count.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_apu_logical_cpus_6041(void);
 *     - Returns the soft compile-time logical CPU (hardware thread)
 *       count for the Steam Deck Van Gogh APU topology stub (always 8:
 *       4 Zen 2 cores × 2 SMT). Not a runtime /sys or CPUID probe.
 *   uint32_t __gj_deck_apu_logical_cpus_6041  (alias)
 *   __libcgj_batch6041_marker = "libcgj-batch6041"
 *
 * Exclusive continuum CREATE-ONLY (6041-6050: CPU core/thread topology
 * stubs for Deck APU). Unique gj_deck_apu_logical_cpus_6041 surface
 * only; no multi-def. Distinct from get_nprocs (batch14). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6041_marker[] = "libcgj-batch6041";

/* Steam Deck Van Gogh: 4c/8t logical hardware threads. */
#define B6041_LOGICAL_CPUS  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6041_logical_cpus(void)
{
	return B6041_LOGICAL_CPUS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_apu_logical_cpus_6041 - soft Deck APU logical CPU count.
 *
 * Always returns 8 (Van Gogh 4c/8t SMT topology stub). Soft pure-data
 * constant; does not call CPUID or read sysfs. No parent wires.
 */
uint32_t
gj_deck_apu_logical_cpus_6041(void)
{
	(void)NULL;
	return b6041_logical_cpus();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_apu_logical_cpus_6041(void)
    __attribute__((alias("gj_deck_apu_logical_cpus_6041")));
