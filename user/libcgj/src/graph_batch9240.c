/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9240: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9240(void);
 *     - Returns the compile-time graph batch number for this TU (9240).
 *   uint32_t __gj_batch_id_9240  (alias)
 *   __libcgj_batch9240_marker = "libcgj-batch9240"
 *
 * Exclusive continuum CREATE-ONLY (9231-9240: sqlite soft id stubs —
 * ok_id, error_id, busy_id, row_id, done_id, open_ready_u,
 * exec_ready_u, stmt_ok_u, ready_u, batch_id_9240). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. Soft pure-data catalog only; no SQLite I/O. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9240_marker[] = "libcgj-batch9240";

#define B9240_BATCH_ID  9240u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9240_id(void)
{
	return B9240_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9240 - report this TU's graph batch number.
 *
 * Always returns 9240. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_9240(void)
{
	(void)NULL;
	return b9240_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9240(void)
    __attribute__((alias("gj_batch_id_9240")));
