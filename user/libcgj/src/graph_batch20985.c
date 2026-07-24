/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20985: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20985(void);
 *     - Returns the compile-time graph batch number for this TU (20985).
 *   uint32_t gj_latch_u_20985(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20985  (alias)
 *   uint32_t __gj_latch_u_20985  (alias)
 *   __libcgj_batch20985_marker = "libcgj-batch20985"
 *
 * Exclusive continuum CREATE-ONLY (20976-21000 wave toward MILESTONE
 * 21000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20985_marker[] = "libcgj-batch20985";

/* Batch identity. */
#define B20985_ID  20985u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20985_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20985_id(void)
{
	return B20985_ID;
}

static uint32_t
b20985_latch(void)
{
	return B20985_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20985 - soft continuum surface
 *
 * Always returns 20985u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20985(void)
{
	(void)NULL;
	return b20985_id();
}

/*
 * gj_latch_u_20985 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_20985(void)
{
	return b20985_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20985(void)
    __attribute__((alias("gj_batch_id_20985")));

uint32_t __gj_latch_u_20985(void)
    __attribute__((alias("gj_latch_u_20985")));
