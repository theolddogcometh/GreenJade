/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6970: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6970(void);
 *     - Returns the compile-time graph batch number for this TU (6970).
 *   uint32_t __gj_batch_id_6970  (alias)
 *   __libcgj_batch6970_marker = "libcgj-batch6970"
 *
 * Exclusive continuum CREATE-ONLY (6961-6970: mlock2 flags stubs —
 * onfault, flags_zero_ok, flags_mask_ok, mcl_current, mcl_future,
 * mcl_onfault, mcl_pack, mcl_mask_ok, flags_ready, batch_id_6970).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6970_marker[] = "libcgj-batch6970";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6970_id(void)
{
	return 6970u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6970 - report this TU's graph batch number.
 *
 * Always returns 6970. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_6970(void)
{
	(void)NULL;
	return b6970_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6970(void)
    __attribute__((alias("gj_batch_id_6970")));
