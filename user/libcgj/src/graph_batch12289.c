/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12289: bar3 checklist soft aggregate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_checklist_soft_u_12289(void);
 *     - Returns 0 (soft checklist aggregate soft/unprobed for the
 *       12281-12290 bar3 soft-stub wave — all→0; not a runtime hard
 *       probe).
 *   uint32_t __gj_bar3_checklist_soft_u_12289  (alias)
 *   __libcgj_batch12289_marker = "libcgj-batch12289"
 *
 * Exclusive continuum CREATE-ONLY (12281-12290: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_checklist_soft_u_12289 surface only;
 * no multi-def. Distinct from gj_bar3_checklist_soft_u_10089 (returns
 * 1), gj_bar3_checklist_soft_u_12089, the per-slot check_u soft stubs
 * (12281-12288), and gj_bar3_checklist_full_mask_5701. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12289_marker[] = "libcgj-batch12289";

/* Soft checklist aggregate unprobed (all→0 wave). */
#define B12289_CHECKLIST_SOFT  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12289_checklist_soft(void)
{
	return B12289_CHECKLIST_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_checklist_soft_u_12289 - soft bar3 checklist aggregate stub.
 *
 * Always returns 0 (soft/unprobed aggregate for all→0 wave). Does not
 * call libc or hard-probe any slot. No parent wires.
 */
uint32_t
gj_bar3_checklist_soft_u_12289(void)
{
	(void)NULL;
	return b12289_checklist_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_checklist_soft_u_12289(void)
    __attribute__((alias("gj_bar3_checklist_soft_u_12289")));
