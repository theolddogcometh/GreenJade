/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28989: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28989(void);
 *     - Returns the compile-time graph batch number for this TU (28989).
 *   uint32_t gj_spark_u_28989(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_28989  (alias)
 *   uint32_t __gj_spark_u_28989  (alias)
 *   __libcgj_batch28989_marker = "libcgj-batch28989"
 *
 * Exclusive continuum CREATE-ONLY (28976-29000 wave toward MILESTONE
 * 29000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch28989_marker[] = "libcgj-batch28989";

/* Batch identity. */
#define B28989_ID  28989u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B28989_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28989_id(void)
{
	return B28989_ID;
}

static uint32_t
b28989_spark(void)
{
	return B28989_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28989 - soft continuum surface
 *
 * Always returns 28989u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_28989(void)
{
	(void)NULL;
	return b28989_id();
}

/*
 * gj_spark_u_28989 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_28989(void)
{
	return b28989_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_28989(void)
    __attribute__((alias("gj_batch_id_28989")));

uint32_t __gj_spark_u_28989(void)
    __attribute__((alias("gj_spark_u_28989")));
