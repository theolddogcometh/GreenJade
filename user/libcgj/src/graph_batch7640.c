/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7640: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7640(void);
 *     - Returns the compile-time graph batch number for this TU (7640).
 *   uint32_t __gj_batch_id_7640  (alias)
 *   __libcgj_batch7640_marker = "libcgj-batch7640"
 *
 * Exclusive continuum CREATE-ONLY (7631-7640: getrusage who stubs —
 * self_id, children_id, thread_id, is_self, is_children, is_thread,
 * who_ok, who_errorish, who_count, batch_id_7640). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7640_marker[] = "libcgj-batch7640";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7640_id(void)
{
	return 7640u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7640 - report this TU's graph batch number.
 *
 * Always returns 7640. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7640(void)
{
	(void)NULL;
	return b7640_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7640(void)
    __attribute__((alias("gj_batch_id_7640")));
