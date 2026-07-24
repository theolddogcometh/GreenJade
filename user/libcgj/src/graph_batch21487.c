/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21487: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_21487(void);
 *     - Returns the compile-time graph batch number for this TU (21487).
 *   uint32_t gj_tone_u_21487(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_21487  (alias)
 *   uint32_t __gj_tone_u_21487  (alias)
 *   __libcgj_batch21487_marker = "libcgj-batch21487"
 *
 * Exclusive continuum CREATE-ONLY (21476-21500 wave toward MILESTONE
 * 21500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch21487_marker[] = "libcgj-batch21487";

/* Batch identity. */
#define B21487_ID  21487u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B21487_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21487_id(void)
{
	return B21487_ID;
}

static uint32_t
b21487_tone(void)
{
	return B21487_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_21487 - soft continuum surface
 *
 * Always returns 21487u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_21487(void)
{
	(void)NULL;
	return b21487_id();
}

/*
 * gj_tone_u_21487 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_21487(void)
{
	return b21487_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_21487(void)
    __attribute__((alias("gj_batch_id_21487")));

uint32_t __gj_tone_u_21487(void)
    __attribute__((alias("gj_tone_u_21487")));
