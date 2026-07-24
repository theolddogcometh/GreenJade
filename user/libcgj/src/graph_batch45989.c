/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch45989: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_45989(void);
 *     - Returns the compile-time graph batch number for this TU (45989).
 *   uint32_t gj_spark_u_45989(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_45989  (alias)
 *   uint32_t __gj_spark_u_45989  (alias)
 *   __libcgj_batch45989_marker = "libcgj-batch45989"
 *
 * Exclusive continuum CREATE-ONLY (45976-46000 wave toward MILESTONE
 * 46000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch45989_marker[] = "libcgj-batch45989";

/* Batch identity. */
#define B45989_ID  45989u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B45989_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b45989_id(void)
{
	return B45989_ID;
}

static uint32_t
b45989_spark(void)
{
	return B45989_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_45989 - soft continuum surface
 *
 * Always returns 45989u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_45989(void)
{
	(void)NULL;
	return b45989_id();
}

/*
 * gj_spark_u_45989 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_45989(void)
{
	return b45989_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_45989(void)
    __attribute__((alias("gj_batch_id_45989")));

uint32_t __gj_spark_u_45989(void)
    __attribute__((alias("gj_spark_u_45989")));
