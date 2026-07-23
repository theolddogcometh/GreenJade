/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7616: getrlimit RLIMIT_CPU resource id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rlim_cpu_id_7616(void);
 *     - Return soft RLIMIT_CPU resource id (0).
 *   uint32_t __gj_rlim_cpu_id_7616  (alias)
 *   __libcgj_batch7616_marker = "libcgj-batch7616"
 *
 * Exclusive continuum CREATE-ONLY (7611-7620: getrlimit resource stubs —
 * infinity, nlimits, resource_ok, is_inf, soft_le_hard, cpu_id,
 * nofile_id, as_id, pair_ok, continuum + batch_id_7620). Unique
 * gj_rlim_cpu_id_7616 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7616_marker[] = "libcgj-batch7616";

/* Soft RLIMIT_CPU on Linux. */
#define B7616_RLIMIT_CPU  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7616_rlim_cpu_id(void)
{
	return B7616_RLIMIT_CPU;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rlim_cpu_id_7616 - soft RLIMIT_CPU resource catalog constant.
 *
 * Always returns 0. Catalog id only; does not call getrlimit.
 * No parent wires.
 */
uint32_t
gj_rlim_cpu_id_7616(void)
{
	(void)NULL;
	return b7616_rlim_cpu_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rlim_cpu_id_7616(void)
    __attribute__((alias("gj_rlim_cpu_id_7616")));
