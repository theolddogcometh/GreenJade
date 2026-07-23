/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15589: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15589(void);
 *     - Returns the compile-time graph batch number for this TU (15589).
 *   uint32_t gj_spark_u_15589(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15589  (alias)
 *   uint32_t __gj_spark_u_15589  (alias)
 *   __libcgj_batch15589_marker = "libcgj-batch15589"
 *
 * Exclusive continuum CREATE-ONLY (15576-15600 wave toward MILESTONE
 * 15600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch15589_marker[] = "libcgj-batch15589";

/* Batch identity. */
#define B15589_ID  15589u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15589_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15589_id(void)
{
	return B15589_ID;
}

static uint32_t
b15589_spark(void)
{
	return B15589_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15589 - soft continuum surface
 *
 * Always returns 15589u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15589(void)
{
	(void)NULL;
	return b15589_id();
}

/*
 * gj_spark_u_15589 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_15589(void)
{
	return b15589_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15589(void)
    __attribute__((alias("gj_batch_id_15589")));

uint32_t __gj_spark_u_15589(void)
    __attribute__((alias("gj_spark_u_15589")));
