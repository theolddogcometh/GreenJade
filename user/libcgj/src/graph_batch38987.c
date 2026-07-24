/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch38987: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_38987(void);
 *     - Returns the compile-time graph batch number for this TU (38987).
 *   uint32_t gj_tone_u_38987(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_38987  (alias)
 *   uint32_t __gj_tone_u_38987  (alias)
 *   __libcgj_batch38987_marker = "libcgj-batch38987"
 *
 * Exclusive continuum CREATE-ONLY (38976-39000 wave toward MILESTONE
 * 39000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch38987_marker[] = "libcgj-batch38987";

/* Batch identity. */
#define B38987_ID  38987u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B38987_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b38987_id(void)
{
	return B38987_ID;
}

static uint32_t
b38987_tone(void)
{
	return B38987_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_38987 - soft continuum surface
 *
 * Always returns 38987u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_38987(void)
{
	(void)NULL;
	return b38987_id();
}

/*
 * gj_tone_u_38987 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_38987(void)
{
	return b38987_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_38987(void)
    __attribute__((alias("gj_batch_id_38987")));

uint32_t __gj_tone_u_38987(void)
    __attribute__((alias("gj_tone_u_38987")));
