/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9080: pixman soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9080(void);
 *     - Returns the compile-time graph batch number for this TU (9080).
 *   uint32_t __gj_batch_id_9080  (alias)
 *   __libcgj_batch9080_marker = "libcgj-batch9080"
 *
 * Exclusive continuum CREATE-ONLY (9071-9080: pixman soft id stubs —
 * a8r8g8b8_id_9071, x8r8g8b8_id_9072, r5g6b5_id_9073, a8_id_9074,
 * op_over_id_9075, op_src_id_9076, region_ok_u_9077, image_ok_u_9078,
 * ready_u_9079, batch_id_9080). Unique surfaces only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9080_marker[] = "libcgj-batch9080";

#define B9080_BATCH_ID  9080u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9080_id(void)
{
	return B9080_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9080 - report this TU's graph batch number.
 *
 * Always returns 9080.
 */
uint32_t
gj_batch_id_9080(void)
{
	(void)NULL;
	return b9080_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9080(void)
    __attribute__((alias("gj_batch_id_9080")));
