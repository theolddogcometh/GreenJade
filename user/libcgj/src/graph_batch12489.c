/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12489: bar3 checklist soft aggregate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_checklist_soft_u_12489(void);
 *     - Returns 0 (soft checklist aggregate soft/unprobed for the
 *       12481-12490 bar3 soft-stub wave — all→0; not a runtime hard
 *       probe).
 *   uint32_t __gj_bar3_checklist_soft_u_12489  (alias)
 *   __libcgj_batch12489_marker = "libcgj-batch12489"
 *
 * Exclusive continuum CREATE-ONLY (12481-12490: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_checklist_soft_u_12489 surface only;
 * no multi-def. Distinct from gj_bar3_checklist_soft_u_10089 (returns
 * 1), gj_bar3_checklist_soft_u_12289, the per-slot check_u soft stubs
 * (12481-12488), and gj_bar3_checklist_full_mask_5701. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12489_marker[] = "libcgj-batch12489";

/* Soft checklist aggregate unprobed (all→0 wave). */
#define B12489_CHECKLIST_SOFT  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12489_checklist_soft(void)
{
	return B12489_CHECKLIST_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_checklist_soft_u_12489 - soft bar3 checklist aggregate stub.
 *
 * Always returns 0 (soft/unprobed aggregate for all→0 wave). Does not
 * call libc or hard-probe any slot. No parent wires.
 */
uint32_t
gj_bar3_checklist_soft_u_12489(void)
{
	(void)NULL;
	return b12489_checklist_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_checklist_soft_u_12489(void)
    __attribute__((alias("gj_bar3_checklist_soft_u_12489")));
