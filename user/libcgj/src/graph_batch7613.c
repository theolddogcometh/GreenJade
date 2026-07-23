/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7613: getrlimit resource-id range stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rlim_resource_ok_u_7613(uint32_t resource);
 *     - Return 1 if resource is in [0, RLIMIT_NLIMITS), else 0.
 *   uint32_t __gj_rlim_resource_ok_u_7613  (alias)
 *   __libcgj_batch7613_marker = "libcgj-batch7613"
 *
 * Exclusive continuum CREATE-ONLY (7611-7620: getrlimit resource stubs —
 * infinity, nlimits, resource_ok, is_inf, soft_le_hard, cpu_id,
 * nofile_id, as_id, pair_ok, continuum + batch_id_7620). Unique
 * gj_rlim_resource_ok_u_7613 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7613_marker[] = "libcgj-batch7613";

/* Soft RLIMIT_NLIMITS on Linux. */
#define B7613_RLIMIT_NLIMITS  ((uint32_t)16u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7613_rlim_resource_ok(uint32_t u32Resource)
{
	return (u32Resource < B7613_RLIMIT_NLIMITS) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rlim_resource_ok_u_7613 - 1 if resource id is in soft Linux range.
 *
 * resource: soft getrlimit/setrlimit resource argument
 *
 * Returns 1 when resource is in [0, 16), else 0. Pure range test;
 * no parent wires. Does not call getrlimit.
 */
uint32_t
gj_rlim_resource_ok_u_7613(uint32_t u32Resource)
{
	(void)NULL;
	return b7613_rlim_resource_ok(u32Resource);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rlim_resource_ok_u_7613(uint32_t u32Resource)
    __attribute__((alias("gj_rlim_resource_ok_u_7613")));
