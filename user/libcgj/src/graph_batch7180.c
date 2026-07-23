/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7180: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7180(void);
 *     - Returns the compile-time graph batch number for this TU (7180).
 *   uint32_t __gj_batch_id_7180  (alias)
 *   __libcgj_batch7180_marker = "libcgj-batch7180"
 *
 * Exclusive continuum CREATE-ONLY (7171-7180: dlopen flag stubs —
 * lazy_id, now_id, global_id, has_lazy, has_now, has_global,
 * flags_ok, flags_pack, flags_errorish, batch_id_7180).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7180_marker[] = "libcgj-batch7180";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7180_id(void)
{
	return 7180u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7180 - report this TU's graph batch number.
 *
 * Always returns 7180. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7180(void)
{
	(void)NULL;
	return b7180_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7180(void)
    __attribute__((alias("gj_batch_id_7180")));
