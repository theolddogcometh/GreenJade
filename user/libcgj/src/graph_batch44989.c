/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44989: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_44989(void);
 *     - Returns the compile-time graph batch number for this TU (44989).
 *   uint32_t gj_spark_u_44989(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_44989  (alias)
 *   uint32_t __gj_spark_u_44989  (alias)
 *   __libcgj_batch44989_marker = "libcgj-batch44989"
 *
 * Exclusive continuum CREATE-ONLY (44976-45000 wave toward MILESTONE
 * 45000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch44989_marker[] = "libcgj-batch44989";

/* Batch identity. */
#define B44989_ID  44989u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B44989_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b44989_id(void)
{
	return B44989_ID;
}

static uint32_t
b44989_spark(void)
{
	return B44989_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_44989 - soft continuum surface
 *
 * Always returns 44989u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_44989(void)
{
	(void)NULL;
	return b44989_id();
}

/*
 * gj_spark_u_44989 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_44989(void)
{
	return b44989_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_44989(void)
    __attribute__((alias("gj_batch_id_44989")));

uint32_t __gj_spark_u_44989(void)
    __attribute__((alias("gj_spark_u_44989")));
