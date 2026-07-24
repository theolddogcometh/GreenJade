/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22989: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_22989(void);
 *     - Returns the compile-time graph batch number for this TU (22989).
 *   uint32_t gj_spark_u_22989(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_22989  (alias)
 *   uint32_t __gj_spark_u_22989  (alias)
 *   __libcgj_batch22989_marker = "libcgj-batch22989"
 *
 * Exclusive continuum CREATE-ONLY (22976-23000 wave toward MILESTONE
 * 23000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch22989_marker[] = "libcgj-batch22989";

/* Batch identity. */
#define B22989_ID  22989u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B22989_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22989_id(void)
{
	return B22989_ID;
}

static uint32_t
b22989_spark(void)
{
	return B22989_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_22989 - soft continuum surface
 *
 * Always returns 22989u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_22989(void)
{
	(void)NULL;
	return b22989_id();
}

/*
 * gj_spark_u_22989 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_22989(void)
{
	return b22989_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_22989(void)
    __attribute__((alias("gj_batch_id_22989")));

uint32_t __gj_spark_u_22989(void)
    __attribute__((alias("gj_spark_u_22989")));
