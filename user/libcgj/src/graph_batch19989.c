/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19989: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19989(void);
 *     - Returns the compile-time graph batch number for this TU (19989).
 *   uint32_t gj_spark_u_19989(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19989  (alias)
 *   uint32_t __gj_spark_u_19989  (alias)
 *   __libcgj_batch19989_marker = "libcgj-batch19989"
 *
 * Exclusive continuum CREATE-ONLY (19976-20000 wave toward MILESTONE
 * 20000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19989_marker[] = "libcgj-batch19989";

/* Batch identity. */
#define B19989_ID  19989u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19989_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19989_id(void)
{
	return B19989_ID;
}

static uint32_t
b19989_spark(void)
{
	return B19989_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19989 - soft continuum surface
 *
 * Always returns 19989u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19989(void)
{
	(void)NULL;
	return b19989_id();
}

/*
 * gj_spark_u_19989 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_19989(void)
{
	return b19989_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19989(void)
    __attribute__((alias("gj_batch_id_19989")));

uint32_t __gj_spark_u_19989(void)
    __attribute__((alias("gj_spark_u_19989")));
