/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6049: Deck APU NUMA node count stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_apu_numa_nodes_6049(void);
 *     - Returns soft NUMA node count for the Steam Deck Van Gogh APU
 *       topology stub (always 1 — unified on-package memory). Not a
 *       runtime /sys/devices/system/node probe.
 *   uint32_t __gj_deck_apu_numa_nodes_6049  (alias)
 *   __libcgj_batch6049_marker = "libcgj-batch6049"
 *
 * Exclusive continuum CREATE-ONLY (6041-6050: CPU core/thread topology
 * stubs for Deck APU). Unique gj_deck_apu_numa_nodes_6049 surface only;
 * no multi-def. Distinct from package/CCX stubs 6044/6045. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6049_marker[] = "libcgj-batch6049";

/* Steam Deck Van Gogh: single NUMA node (UMA SoC). */
#define B6049_NUMA_NODES  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6049_numa_nodes(void)
{
	return B6049_NUMA_NODES;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_apu_numa_nodes_6049 - soft Deck APU NUMA node count.
 *
 * Always returns 1 (single-node UMA topology stub). Soft pure-data
 * constant; does not probe sysfs. No parent wires.
 */
uint32_t
gj_deck_apu_numa_nodes_6049(void)
{
	(void)NULL;
	return b6049_numa_nodes();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_apu_numa_nodes_6049(void)
    __attribute__((alias("gj_deck_apu_numa_nodes_6049")));
