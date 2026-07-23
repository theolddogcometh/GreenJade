/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4070: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4070(void);
 *     - Returns the compile-time graph batch number for this TU (4070).
 *   uint32_t __gj_batch_id_4070  (alias)
 *   __libcgj_batch4070_marker = "libcgj-batch4070"
 *
 * Exclusive continuum CREATE-ONLY (4061-4070): u64_remap_u,
 * u32_smoothstep_u, u32_smootherstep_u, u32_step_u, u64_step_u,
 * u32_pulse_u, u64_pulse_u, u32_saw_u, u64_saw_u, batch_id_4070.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4070_marker[] = "libcgj-batch4070";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4070_id(void)
{
	return 4070u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4070 - report this TU's graph batch number.
 *
 * Always returns 4070.
 */
uint32_t
gj_batch_id_4070(void)
{
	(void)NULL;
	return b4070_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4070(void)
    __attribute__((alias("gj_batch_id_4070")));
