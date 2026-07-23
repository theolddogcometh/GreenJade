/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8040: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8040(void);
 *     - Returns the compile-time graph batch number for this TU (8040).
 *   uint32_t __gj_batch_id_8040  (alias)
 *   __libcgj_batch8040_marker = "libcgj-batch8040"
 *
 * Exclusive continuum CREATE-ONLY (8031-8040: free null ok stubs —
 * null_ok, null_noop, null_errno_ok, action_noop, action_reclaim,
 * null_ok_u, should_skip_u, action_kind_u, null_policy, batch_id_8040).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8040_marker[] = "libcgj-batch8040";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8040_id(void)
{
return 8040u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8040 - report this TU's graph batch number.
 *
 * Always returns 8040. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8040(void)
{
(void)NULL;
return b8040_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8040(void)
    __attribute__((alias("gj_batch_id_8040")));
