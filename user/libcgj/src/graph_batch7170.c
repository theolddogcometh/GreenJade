/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7170: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7170(void);
 *     - Returns the compile-time graph batch number for this TU (7170).
 *   uint32_t __gj_batch_id_7170  (alias)
 *   __libcgj_batch7170_marker = "libcgj-batch7170"
 *
 * Exclusive continuum CREATE-ONLY (7161-7170: pthread attr detach stubs —
 * joinable_id, detached_id, is_joinable, is_detached, detach_ok,
 * detach_pack, detach_errorish, detach_default, detach_normalize,
 * batch_id_7170).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7170_marker[] = "libcgj-batch7170";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7170_id(void)
{
	return 7170u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7170 - report this TU's graph batch number.
 *
 * Always returns 7170. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7170(void)
{
	(void)NULL;
	return b7170_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7170(void)
    __attribute__((alias("gj_batch_id_7170")));
