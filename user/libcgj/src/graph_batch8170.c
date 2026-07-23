/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8170: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8170(void);
 *     - Returns the compile-time graph batch number for this TU (8170).
 *   uint32_t __gj_batch_id_8170  (alias)
 *   __libcgj_batch8170_marker = "libcgj-batch8170"
 *
 * Exclusive continuum CREATE-ONLY (8161-8170: strerror errno stubs —
 * success, eperm, enoent, einval, enomem, enosys, known_ok, table_n,
 * r_min_buf, batch_id_8170).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8170_marker[] = "libcgj-batch8170";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8170_id(void)
{
	return 8170u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8170 - report this TU's graph batch number.
 *
 * Always returns 8170. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8170(void)
{
	(void)NULL;
	return b8170_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8170(void)
    __attribute__((alias("gj_batch_id_8170")));
