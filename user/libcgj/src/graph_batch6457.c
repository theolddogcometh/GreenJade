/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6457: achievement unlock mask popcount stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ach_popcount_6457(uint32_t mask);
 *     - Count set bits in the unlock mask (unlocked achievement count
 *       in a 32-slot soft bank). Returns 0..32.
 *   uint32_t __gj_ach_popcount_6457  (alias)
 *   __libcgj_batch6457_marker = "libcgj-batch6457"
 *
 * Exclusive continuum CREATE-ONLY (6451-6460: achievement unlock mask
 * stubs). Unique gj_ach_popcount_6457 surface only; no multi-def.
 * Self-contained SWAR popcount; no libc / no builtins required.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6457_marker[] = "libcgj-batch6457";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6457_popcount(uint32_t uMask)
{
	uint32_t u;

	u = uMask;
	u = u - ((u >> 1u) & 0x55555555u);
	u = (u & 0x33333333u) + ((u >> 2u) & 0x33333333u);
	u = (u + (u >> 4u)) & 0x0f0f0f0fu;
	u = u + (u >> 8u);
	u = u + (u >> 16u);
	return u & 0x3fu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ach_popcount_6457 - count unlocked achievement bits in mask.
 *
 * mask: unlock bitfield
 * Returns number of set bits in [0, 32]. Pure integer SWAR popcount.
 * No parent wires.
 */
uint32_t
gj_ach_popcount_6457(uint32_t mask)
{
	(void)NULL;
	return b6457_popcount(mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ach_popcount_6457(uint32_t mask)
    __attribute__((alias("gj_ach_popcount_6457")));
