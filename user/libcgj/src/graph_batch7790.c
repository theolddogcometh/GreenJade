/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7790: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7790(void);
 *     - Returns the compile-time graph batch number for this TU (7790).
 *   uint32_t __gj_batch_id_7790  (alias)
 *   __libcgj_batch7790_marker = "libcgj-batch7790"
 *
 * Exclusive continuum CREATE-ONLY (7781-7790: glob flags stubs —
 * err_id, mark_id, nocheck_id, has_err, has_mark, has_nocheck,
 * flags_ok, flags_pack, flags_errorish, batch_id_7790).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7790_marker[] = "libcgj-batch7790";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7790_id(void)
{
	return 7790u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7790 - report this TU's graph batch number.
 *
 * Always returns 7790. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7790(void)
{
	(void)NULL;
	return b7790_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7790(void)
    __attribute__((alias("gj_batch_id_7790")));
