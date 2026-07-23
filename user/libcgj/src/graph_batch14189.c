/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14189: bar3 checklist soft aggregate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_checklist_soft_u_14189(void);
 *     - Returns 0 (soft checklist aggregate soft/unprobed for the
 *       14181-14190 bar3 soft-stub wave — all→0; not a runtime hard
 *       probe).
 *   uint32_t __gj_bar3_checklist_soft_u_14189  (alias)
 *   __libcgj_batch14189_marker = "libcgj-batch14189"
 *
 * Exclusive continuum CREATE-ONLY (14181-14190: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_checklist_soft_u_14189 surface only;
 * no multi-def. Distinct from gj_bar3_checklist_soft_u_13089,
 * gj_bar3_checklist_soft_u_12889, the per-slot check_u soft stubs
 * (14181-14188), and gj_bar3_checklist_full_mask_5701. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14189_marker[] = "libcgj-batch14189";

/* Soft checklist aggregate unprobed (all→0 wave). */
#define B14189_CHECKLIST_SOFT  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14189_checklist_soft(void)
{
	return B14189_CHECKLIST_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_checklist_soft_u_14189 - soft bar3 checklist aggregate stub.
 *
 * Always returns 0 (soft/unprobed aggregate for all→0 wave). Does not
 * call libc or hard-probe any slot. No parent wires.
 */
uint32_t
gj_bar3_checklist_soft_u_14189(void)
{
	(void)NULL;
	return b14189_checklist_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_checklist_soft_u_14189(void)
    __attribute__((alias("gj_bar3_checklist_soft_u_14189")));
