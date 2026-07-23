/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7611: getrlimit RLIM_INFINITY soft constant stub.
 *
 * Surface (unique symbols):
 *   uint64_t gj_rlim_infinity_u64_7611(void);
 *     - Return soft RLIM_INFINITY (all-ones rlim_t / UINT64_MAX).
 *   uint64_t __gj_rlim_infinity_u64_7611  (alias)
 *   __libcgj_batch7611_marker = "libcgj-batch7611"
 *
 * Exclusive continuum CREATE-ONLY (7611-7620: getrlimit resource stubs —
 * infinity, nlimits, resource_ok, is_inf, soft_le_hard, cpu_id,
 * nofile_id, as_id, pair_ok, continuum + batch_id_7620). Unique
 * gj_rlim_infinity_u64_7611 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7611_marker[] = "libcgj-batch7611";

/* Soft RLIM_INFINITY: (rlim_t)-1 on 64-bit Linux. */
#define B7611_RLIM_INFINITY  ((uint64_t)0xffffffffffffffffull)

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b7611_rlim_infinity(void)
{
	return B7611_RLIM_INFINITY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rlim_infinity_u64_7611 - soft RLIM_INFINITY catalog constant.
 *
 * Always returns UINT64_MAX (soft all-ones rlim_t). Catalog id only;
 * does not call getrlimit/setrlimit. No parent wires.
 */
uint64_t
gj_rlim_infinity_u64_7611(void)
{
	(void)NULL;
	return b7611_rlim_infinity();
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rlim_infinity_u64_7611(void)
    __attribute__((alias("gj_rlim_infinity_u64_7611")));
