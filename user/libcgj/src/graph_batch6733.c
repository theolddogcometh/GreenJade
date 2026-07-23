/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6733: prctl PR_GET_DUMPABLE option stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prctl_get_dumpable_6733(void);
 *     - Returns 3 (soft PR_GET_DUMPABLE option id). Pure-data catalog
 *       stub; not a live prctl(2) call.
 *   uint32_t __gj_prctl_get_dumpable_6733  (alias)
 *   __libcgj_batch6733_marker = "libcgj-batch6733"
 *
 * Exclusive continuum CREATE-ONLY (6731-6740: prctl option stubs —
 * set_pdeathsig_6731, get_pdeathsig_6732, get_dumpable_6733,
 * set_dumpable_6734, get_keepcaps_6735, set_keepcaps_6736,
 * set_name_6737, get_name_6738, option_ok_6739,
 * continuum + batch_id_6740). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6733_marker[] = "libcgj-batch6733";

/* Soft PR_GET_DUMPABLE (get dumpable flag). */
#define B6733_PR_GET_DUMPABLE  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6733_get_dumpable(void)
{
	return B6733_PR_GET_DUMPABLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prctl_get_dumpable_6733 - soft PR_GET_DUMPABLE option catalog id.
 *
 * Always returns 3. Pure-data stub for product prctl option catalogs;
 * does not invoke prctl or inspect dumpability. No parent wires.
 */
uint32_t
gj_prctl_get_dumpable_6733(void)
{
	(void)NULL;
	return b6733_get_dumpable();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prctl_get_dumpable_6733(void)
    __attribute__((alias("gj_prctl_get_dumpable_6733")));
