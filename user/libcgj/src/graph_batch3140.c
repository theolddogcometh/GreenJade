/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3140: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3140(void);
 *     - Returns the compile-time graph batch number for this TU (3140).
 *   uint32_t __gj_batch_id_3140  (alias)
 *   __libcgj_batch3140_marker = "libcgj-batch3140"
 *
 * Exclusive continuum CREATE-ONLY (3131-3140): time_ms_to_us_u,
 * time_us_to_ms_u, time_s_to_ms_u, time_ms_to_s_u, duration_add_sat_u,
 * duration_sub_sat_u, deadline_passed_u, timeout_remaining_u,
 * hz_to_period_ns_u, batch_id_3140. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3140_marker[] = "libcgj-batch3140";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3140_id(void)
{
	return 3140u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3140 - report this TU's graph batch number.
 *
 * Always returns 3140.
 */
uint32_t
gj_batch_id_3140(void)
{
	(void)NULL;
	return b3140_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3140(void)
    __attribute__((alias("gj_batch_id_3140")));
