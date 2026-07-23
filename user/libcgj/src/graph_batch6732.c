/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6732: prctl PR_GET_PDEATHSIG option stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prctl_get_pdeathsig_6732(void);
 *     - Returns 2 (soft PR_GET_PDEATHSIG option id). Pure-data catalog
 *       stub; not a live prctl(2) call.
 *   uint32_t __gj_prctl_get_pdeathsig_6732  (alias)
 *   __libcgj_batch6732_marker = "libcgj-batch6732"
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

const char __libcgj_batch6732_marker[] = "libcgj-batch6732";

/* Soft PR_GET_PDEATHSIG (get parent-death signal). */
#define B6732_PR_GET_PDEATHSIG  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6732_get_pdeathsig(void)
{
	return B6732_PR_GET_PDEATHSIG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prctl_get_pdeathsig_6732 - soft PR_GET_PDEATHSIG option catalog id.
 *
 * Always returns 2. Pure-data stub for product prctl option catalogs;
 * does not invoke prctl or read signal state. No parent wires.
 */
uint32_t
gj_prctl_get_pdeathsig_6732(void)
{
	(void)NULL;
	return b6732_get_pdeathsig();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prctl_get_pdeathsig_6732(void)
    __attribute__((alias("gj_prctl_get_pdeathsig_6732")));
