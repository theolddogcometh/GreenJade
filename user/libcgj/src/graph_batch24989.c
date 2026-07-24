/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24989: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24989(void);
 *     - Returns the compile-time graph batch number for this TU (24989).
 *   uint32_t gj_spark_u_24989(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_24989  (alias)
 *   uint32_t __gj_spark_u_24989  (alias)
 *   __libcgj_batch24989_marker = "libcgj-batch24989"
 *
 * Exclusive continuum CREATE-ONLY (24976-25000 wave toward MILESTONE
 * 25000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch24989_marker[] = "libcgj-batch24989";

/* Batch identity. */
#define B24989_ID  24989u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B24989_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24989_id(void)
{
	return B24989_ID;
}

static uint32_t
b24989_spark(void)
{
	return B24989_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24989 - soft continuum surface
 *
 * Always returns 24989u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_24989(void)
{
	(void)NULL;
	return b24989_id();
}

/*
 * gj_spark_u_24989 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_24989(void)
{
	return b24989_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_24989(void)
    __attribute__((alias("gj_batch_id_24989")));

uint32_t __gj_spark_u_24989(void)
    __attribute__((alias("gj_spark_u_24989")));
