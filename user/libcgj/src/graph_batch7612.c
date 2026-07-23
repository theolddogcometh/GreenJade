/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7612: getrlimit RLIMIT_NLIMITS soft constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rlim_nlimits_id_7612(void);
 *     - Return soft RLIMIT_NLIMITS (16 on Linux).
 *   uint32_t __gj_rlim_nlimits_id_7612  (alias)
 *   __libcgj_batch7612_marker = "libcgj-batch7612"
 *
 * Exclusive continuum CREATE-ONLY (7611-7620: getrlimit resource stubs —
 * infinity, nlimits, resource_ok, is_inf, soft_le_hard, cpu_id,
 * nofile_id, as_id, pair_ok, continuum + batch_id_7620). Unique
 * gj_rlim_nlimits_id_7612 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7612_marker[] = "libcgj-batch7612";

/* Soft RLIMIT_NLIMITS on Linux (resource ids are 0 .. NLIMITS-1). */
#define B7612_RLIMIT_NLIMITS  ((uint32_t)16u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7612_rlim_nlimits(void)
{
	return B7612_RLIMIT_NLIMITS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rlim_nlimits_id_7612 - soft RLIMIT_NLIMITS catalog constant.
 *
 * Always returns 16. Catalog bound only; does not call getrlimit.
 * No parent wires.
 */
uint32_t
gj_rlim_nlimits_id_7612(void)
{
	(void)NULL;
	return b7612_rlim_nlimits();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rlim_nlimits_id_7612(void)
    __attribute__((alias("gj_rlim_nlimits_id_7612")));
