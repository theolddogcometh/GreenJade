/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7614: getrlimit RLIM_INFINITY predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rlim_is_inf_u64_7614(uint64_t val);
 *     - Return 1 if val equals soft RLIM_INFINITY, else 0.
 *   uint32_t __gj_rlim_is_inf_u64_7614  (alias)
 *   __libcgj_batch7614_marker = "libcgj-batch7614"
 *
 * Exclusive continuum CREATE-ONLY (7611-7620: getrlimit resource stubs —
 * infinity, nlimits, resource_ok, is_inf, soft_le_hard, cpu_id,
 * nofile_id, as_id, pair_ok, continuum + batch_id_7620). Unique
 * gj_rlim_is_inf_u64_7614 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7614_marker[] = "libcgj-batch7614";

/* Soft RLIM_INFINITY: (rlim_t)-1 on 64-bit Linux. */
#define B7614_RLIM_INFINITY  ((uint64_t)0xffffffffffffffffull)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7614_rlim_is_inf(uint64_t u64Val)
{
	return (u64Val == B7614_RLIM_INFINITY) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rlim_is_inf_u64_7614 - 1 if val is soft RLIM_INFINITY.
 *
 * val: soft rlim_cur or rlim_max candidate
 *
 * Returns 1 when val equals all-ones (UINT64_MAX), else 0.
 * Self-contained; does not call sibling helpers. No parent wires.
 * Does not call getrlimit.
 */
uint32_t
gj_rlim_is_inf_u64_7614(uint64_t u64Val)
{
	(void)NULL;
	return b7614_rlim_is_inf(u64Val);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rlim_is_inf_u64_7614(uint64_t u64Val)
    __attribute__((alias("gj_rlim_is_inf_u64_7614")));
