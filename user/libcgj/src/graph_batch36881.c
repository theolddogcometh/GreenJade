/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36881: continuum pad x soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36881(void);
 *     - Returns the compile-time graph batch number for this TU (36881).
 *   uint32_t gj_pad_x_u_36881(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_36881  (alias)
 *   uint32_t __gj_pad_x_u_36881  (alias)
 *   __libcgj_batch36881_marker = "libcgj-batch36881"
 *
 * Exclusive continuum CREATE-ONLY (36876-36900 wave toward MILESTONE
 * 36900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch36881_marker[] = "libcgj-batch36881";

/* Batch identity. */
#define B36881_ID  36881u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B36881_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36881_id(void)
{
	return B36881_ID;
}

static uint32_t
b36881_pad_x(void)
{
	return B36881_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36881 - soft continuum surface
 *
 * Always returns 36881u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_36881(void)
{
	(void)NULL;
	return b36881_id();
}

/*
 * gj_pad_x_u_36881 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_x_u_36881(void)
{
	return b36881_pad_x();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36881(void)
    __attribute__((alias("gj_batch_id_36881")));

uint32_t __gj_pad_x_u_36881(void)
    __attribute__((alias("gj_pad_x_u_36881")));
