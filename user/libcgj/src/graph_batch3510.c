/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3510: milestone 3510 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3510(void);
 *     — Returns the compile-time graph batch number for this TU (3510).
 *   uint32_t __gj_batch_id_3510  (alias)
 *   __libcgj_batch3510_marker = "libcgj-batch3510"
 *
 * Milestone for the post-3500 modular/number-theory exclusive wave
 * (batches 3501–3509: u32_gcd_euclid_u, u64_gcd_euclid_u, u32_lcm_safe_u,
 * u64_lcm_safe_u, u32_mod_add_u, u64_mod_add_u, u32_mod_sub_u,
 * u64_mod_sub_u, u32_mod_mul_u). Does NOT redefine gj_batch_id / prior
 * batch_id_* or milestone_* symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3510_marker[] = "libcgj-batch3510";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3510_id(void)
{
	return 3510u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3510 — report this TU's graph batch number.
 *
 * Always returns 3510. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_3510(void)
{
	(void)NULL;
	return b3510_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3510(void)
    __attribute__((alias("gj_batch_id_3510")));
