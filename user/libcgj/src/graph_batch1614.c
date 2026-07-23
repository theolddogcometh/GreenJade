/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1614: map static prio 0..139 to nice-like u.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prio_to_nice_u(uint32_t prio);
 *     — Linux-style static priority 0..139 → unsigned nice-like slot
 *       0..39 (nice + 20). RT band prio 0..99 maps to 0 (most favored
 *       fair slot). Fair band 100..139 maps to 0..39. prio > 139 clamps
 *       to 39 (least favored).
 *   uint32_t __gj_prio_to_nice_u  (alias)
 *   __libcgj_batch1614_marker = "libcgj-batch1614"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1614_marker[] = "libcgj-batch1614";

/* MAX_RT_PRIO-style boundary: fair priorities start here. */
#define B1614_FAIR_PRIO_BASE 100u
#define B1614_PRIO_MAX       139u
#define B1614_NICE_U_MAX     39u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prio_to_nice_u — convert static priority to unsigned nice-like rank.
 *
 * nice_u 0  ↔ nice -20 (highest fair share)
 * nice_u 20 ↔ nice   0
 * nice_u 39 ↔ nice  19 (lowest fair share)
 */
uint32_t
gj_prio_to_nice_u(uint32_t u32Prio)
{
	if (u32Prio < B1614_FAIR_PRIO_BASE) {
		return 0u;
	}
	if (u32Prio > B1614_PRIO_MAX) {
		return B1614_NICE_U_MAX;
	}
	return u32Prio - B1614_FAIR_PRIO_BASE;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prio_to_nice_u(uint32_t u32Prio)
    __attribute__((alias("gj_prio_to_nice_u")));
