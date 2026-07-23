/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2510: milestone 2510 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2510(void);
 *     — Returns the compile-time graph batch number for this TU (2510).
 *   uint32_t __gj_batch_id_2510  (alias)
 *   __libcgj_batch2510_marker = "libcgj-batch2510"
 *
 * Milestone for the post-2500 number-theory exclusive wave (batches
 * 2501–2509: u64_gcd_u, u64_lcm_sat_u, u32_gcd_u, u32_lcm_sat_u,
 * u64_powmod_u, u64_invmod_odd, u64_is_prime_trial_u, u64_next_prime_u,
 * u64_prev_prime_u). Does NOT redefine gj_batch_id / prior batch_id_*
 * or milestone_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2510_marker[] = "libcgj-batch2510";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2510_id(void)
{
	return 2510u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2510 — report this TU's graph batch number.
 *
 * Always returns 2510. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2510(void)
{
	(void)NULL;
	return b2510_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2510(void)
    __attribute__((alias("gj_batch_id_2510")));
