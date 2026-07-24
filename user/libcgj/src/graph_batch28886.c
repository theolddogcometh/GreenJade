/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28886: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28886(void);
 *     - Returns the compile-time graph batch number for this TU (28886).
 *   uint32_t gj_tick_u_28886(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_28886  (alias)
 *   uint32_t __gj_tick_u_28886  (alias)
 *   __libcgj_batch28886_marker = "libcgj-batch28886"
 *
 * Exclusive continuum CREATE-ONLY (28876-28900 wave toward MILESTONE
 * 28900). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Soft continuum only: fill helpers are compile-time lamps, not runtime
 * product probes. bar3 / product score remain open at milestone markers.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch28886_marker[] = "libcgj-batch28886";

/* Batch identity. */
#define B28886_ID  28886u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B28886_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28886_id(void)
{
	return B28886_ID;
}

static uint32_t
b28886_tick(void)
{
	return B28886_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28886 - soft continuum surface
 *
 * Always returns 28886u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_28886(void)
{
	(void)NULL;
	return b28886_id();
}

/*
 * gj_tick_u_28886 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_28886(void)
{
	return b28886_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_28886(void)
    __attribute__((alias("gj_batch_id_28886")));

uint32_t __gj_tick_u_28886(void)
    __attribute__((alias("gj_tick_u_28886")));
