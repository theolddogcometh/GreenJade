/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3630: exclusive wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3630(void);
 *     - Returns the compile-time graph batch number for this TU (3630).
 *   uint32_t __gj_batch_id_3630  (alias)
 *   __libcgj_batch3630_marker = "libcgj-batch3630"
 *
 * Exclusive continuum CREATE-ONLY wave (3621-3630:
 * u64_checked_mod_u, i32_add_sat_u, i64_add_sat_u2, i32_sub_sat_u,
 * i64_sub_sat_u2, i32_mul_sat_u, i64_mul_sat_u2, i32_div_sat_u,
 * i64_div_sat_u, batch_id_3630). Distinct from prior batch_id_*
 * symbols — unique surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3630_marker[] = "libcgj-batch3630";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3630_id(void)
{
	return 3630u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3630 - report this TU's graph batch number.
 *
 * Always returns 3630.
 */
uint32_t
gj_batch_id_3630(void)
{
	(void)NULL;
	return b3630_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3630(void)
    __attribute__((alias("gj_batch_id_3630")));
