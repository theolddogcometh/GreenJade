/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24689: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24689(void);
 *     - Returns the compile-time graph batch number for this TU (24689).
 *   uint32_t gj_spark_u_24689(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_24689  (alias)
 *   uint32_t __gj_spark_u_24689  (alias)
 *   __libcgj_batch24689_marker = "libcgj-batch24689"
 *
 * Exclusive continuum CREATE-ONLY (24676-24700 wave toward MILESTONE
 * 24700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch24689_marker[] = "libcgj-batch24689";

/* Batch identity. */
#define B24689_ID  24689u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B24689_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24689_id(void)
{
	return B24689_ID;
}

static uint32_t
b24689_spark(void)
{
	return B24689_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24689 - soft continuum surface
 *
 * Always returns 24689u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_24689(void)
{
	(void)NULL;
	return b24689_id();
}

/*
 * gj_spark_u_24689 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_24689(void)
{
	return b24689_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_24689(void)
    __attribute__((alias("gj_batch_id_24689")));

uint32_t __gj_spark_u_24689(void)
    __attribute__((alias("gj_spark_u_24689")));
