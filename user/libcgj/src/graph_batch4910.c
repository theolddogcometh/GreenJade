/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4910: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4910(void);
 *     - Returns the compile-time graph batch number for this TU (4910).
 *   uint32_t __gj_batch_id_4910  (alias)
 *   __libcgj_batch4910_marker = "libcgj-batch4910"
 *
 * Exclusive continuum CREATE-ONLY (4901-4910: mat2_det_s, mat2_trace_s,
 * u32_dot2_u, u32_dot3_u, u32_dot4_u, i32_dot2_s, mat2_identity_a_u,
 * mat2_identity_b_u, mat2_scale_u, batch_id_4910). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4910_marker[] = "libcgj-batch4910";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4910_id(void)
{
	return 4910u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4910 - report this TU's graph batch number.
 *
 * Always returns 4910. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4910(void)
{
	(void)NULL;
	return b4910_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4910(void)
    __attribute__((alias("gj_batch_id_4910")));
