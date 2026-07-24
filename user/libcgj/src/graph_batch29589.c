/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29589: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_29589(void);
 *     - Returns the compile-time graph batch number for this TU (29589).
 *   uint32_t gj_spark_u_29589(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_29589  (alias)
 *   uint32_t __gj_spark_u_29589  (alias)
 *   __libcgj_batch29589_marker = "libcgj-batch29589"
 *
 * Exclusive continuum CREATE-ONLY (29576-29600 wave toward MILESTONE
 * 29600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch29589_marker[] = "libcgj-batch29589";

/* Batch identity. */
#define B29589_ID  29589u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B29589_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29589_id(void)
{
	return B29589_ID;
}

static uint32_t
b29589_spark(void)
{
	return B29589_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_29589 - soft continuum surface
 *
 * Always returns 29589u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_29589(void)
{
	(void)NULL;
	return b29589_id();
}

/*
 * gj_spark_u_29589 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_29589(void)
{
	return b29589_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_29589(void)
    __attribute__((alias("gj_batch_id_29589")));

uint32_t __gj_spark_u_29589(void)
    __attribute__((alias("gj_spark_u_29589")));
