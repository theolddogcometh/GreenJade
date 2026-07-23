/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6739: prctl option catalog range check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prctl_option_ok_6739(uint32_t option);
 *     - Returns 1 when option is in soft PR_* catalog range
 *       [1, 17) (classic low PR options through PR_GET_NAME), else 0.
 *       Pure-data range stub; not a live prctl(2) validation.
 *   uint32_t __gj_prctl_option_ok_6739  (alias)
 *   __libcgj_batch6739_marker = "libcgj-batch6739"
 *
 * Exclusive continuum CREATE-ONLY (6731-6740: prctl option stubs —
 * set_pdeathsig_6731, get_pdeathsig_6732, get_dumpable_6733,
 * set_dumpable_6734, get_keepcaps_6735, set_keepcaps_6736,
 * set_name_6737, get_name_6738, option_ok_6739,
 * continuum + batch_id_6740). Unique surface only; no multi-def.
 * Soft bound covers PR_SET_PDEATHSIG..PR_GET_NAME. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6739_marker[] = "libcgj-batch6739";

/* Soft exclusive ceiling after PR_GET_NAME (16); options are 1-based. */
#define B6739_PR_OPT_MIN  1u
#define B6739_PR_OPT_MAX  17u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6739_option_ok(uint32_t uOpt)
{
	return (uOpt >= B6739_PR_OPT_MIN && uOpt < B6739_PR_OPT_MAX)
	    ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prctl_option_ok_6739 - soft classic PR_* option catalog membership.
 *
 * option: candidate prctl option number.
 * Returns 1 if option is in [1, 17), else 0. Does not query the
 * kernel. No parent wires.
 */
uint32_t
gj_prctl_option_ok_6739(uint32_t option)
{
	(void)NULL;
	return b6739_option_ok(option);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prctl_option_ok_6739(uint32_t option)
    __attribute__((alias("gj_prctl_option_ok_6739")));
