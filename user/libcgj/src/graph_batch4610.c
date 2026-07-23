/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4610: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4610(void);
 *     - Returns the compile-time graph batch number for this TU (4610).
 *   uint32_t __gj_batch_id_4610  (alias)
 *   __libcgj_batch4610_marker = "libcgj-batch4610"
 *
 * Exclusive continuum CREATE-ONLY (4601-4610: tick_add_u, tick_sub_u,
 * tick_elapsed_u, tick_expired_u, tick_remaining_u, tick_from_ms_u,
 * tick_to_ms_u, tick_clamp_u, tick_is_zero_u, batch_id_4610).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4610_marker[] = "libcgj-batch4610";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4610_id(void)
{
	return 4610u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4610 - report this TU's graph batch number.
 *
 * Always returns 4610. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4610(void)
{
	(void)NULL;
	return b4610_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4610(void)
    __attribute__((alias("gj_batch_id_4610")));
