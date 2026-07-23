/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7617: getrlimit RLIMIT_NOFILE resource id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rlim_nofile_id_7617(void);
 *     - Return soft RLIMIT_NOFILE resource id (7 on Linux).
 *   uint32_t __gj_rlim_nofile_id_7617  (alias)
 *   __libcgj_batch7617_marker = "libcgj-batch7617"
 *
 * Exclusive continuum CREATE-ONLY (7611-7620: getrlimit resource stubs —
 * infinity, nlimits, resource_ok, is_inf, soft_le_hard, cpu_id,
 * nofile_id, as_id, pair_ok, continuum + batch_id_7620). Unique
 * gj_rlim_nofile_id_7617 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7617_marker[] = "libcgj-batch7617";

/* Soft RLIMIT_NOFILE on Linux. */
#define B7617_RLIMIT_NOFILE  ((uint32_t)7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7617_rlim_nofile_id(void)
{
	return B7617_RLIMIT_NOFILE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rlim_nofile_id_7617 - soft RLIMIT_NOFILE resource catalog constant.
 *
 * Always returns 7. Catalog id only; does not call getrlimit.
 * No parent wires.
 */
uint32_t
gj_rlim_nofile_id_7617(void)
{
	(void)NULL;
	return b7617_rlim_nofile_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rlim_nofile_id_7617(void)
    __attribute__((alias("gj_rlim_nofile_id_7617")));
