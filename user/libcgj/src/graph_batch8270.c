/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8270: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8270(void);
 *     - Returns the compile-time graph batch number for this TU (8270).
 *   uint32_t __gj_batch_id_8270  (alias)
 *   __libcgj_batch8270_marker = "libcgj-batch8270"
 *
 * Exclusive continuum CREATE-ONLY (8261-8270: saturating arithmetic
 * stubs uint32 — sat_add, sat_sub, sat_mul, sat_inc, sat_dec, clamp,
 * min, max, absdiff, batch_id_8270). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8270_marker[] = "libcgj-batch8270";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8270_id(void)
{
	return 8270u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8270 - report this TU's graph batch number.
 *
 * Always returns 8270. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8270(void)
{
	(void)NULL;
	return b8270_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8270(void)
    __attribute__((alias("gj_batch_id_8270")));
