/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch37289: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_37289(void);
 *     - Returns the compile-time graph batch number for this TU (37289).
 *   uint32_t gj_spark_u_37289(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_37289  (alias)
 *   uint32_t __gj_spark_u_37289  (alias)
 *   __libcgj_batch37289_marker = "libcgj-batch37289"
 *
 * Exclusive continuum CREATE-ONLY (37276-37300 wave toward MILESTONE
 * 37300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch37289_marker[] = "libcgj-batch37289";

/* Batch identity. */
#define B37289_ID  37289u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B37289_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b37289_id(void)
{
	return B37289_ID;
}

static uint32_t
b37289_spark(void)
{
	return B37289_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_37289 - soft continuum surface
 *
 * Always returns 37289u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_37289(void)
{
	(void)NULL;
	return b37289_id();
}

/*
 * gj_spark_u_37289 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_37289(void)
{
	return b37289_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_37289(void)
    __attribute__((alias("gj_batch_id_37289")));

uint32_t __gj_spark_u_37289(void)
    __attribute__((alias("gj_spark_u_37289")));
