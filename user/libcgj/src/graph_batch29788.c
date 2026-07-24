/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29788: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_29788(void);
 *     - Returns the compile-time graph batch number for this TU (29788).
 *   uint32_t gj_pulse_u_29788(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_29788  (alias)
 *   uint32_t __gj_pulse_u_29788  (alias)
 *   __libcgj_batch29788_marker = "libcgj-batch29788"
 *
 * Exclusive continuum CREATE-ONLY (29776-29800 wave toward MILESTONE
 * 29800). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch29788_marker[] = "libcgj-batch29788";

/* Batch identity. */
#define B29788_ID  29788u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B29788_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29788_id(void)
{
	return B29788_ID;
}

static uint32_t
b29788_pulse(void)
{
	return B29788_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_29788 - soft continuum surface
 *
 * Always returns 29788u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_29788(void)
{
	(void)NULL;
	return b29788_id();
}

/*
 * gj_pulse_u_29788 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_29788(void)
{
	return b29788_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_29788(void)
    __attribute__((alias("gj_batch_id_29788")));

uint32_t __gj_pulse_u_29788(void)
    __attribute__((alias("gj_pulse_u_29788")));
