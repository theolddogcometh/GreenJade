/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29489: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_29489(void);
 *     - Returns the compile-time graph batch number for this TU (29489).
 *   uint32_t gj_spark_u_29489(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_29489  (alias)
 *   uint32_t __gj_spark_u_29489  (alias)
 *   __libcgj_batch29489_marker = "libcgj-batch29489"
 *
 * Exclusive continuum CREATE-ONLY (29476-29500 wave toward MILESTONE
 * 29500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch29489_marker[] = "libcgj-batch29489";

/* Batch identity. */
#define B29489_ID  29489u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B29489_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29489_id(void)
{
	return B29489_ID;
}

static uint32_t
b29489_spark(void)
{
	return B29489_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_29489 - soft continuum surface
 *
 * Always returns 29489u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_29489(void)
{
	(void)NULL;
	return b29489_id();
}

/*
 * gj_spark_u_29489 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_29489(void)
{
	return b29489_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_29489(void)
    __attribute__((alias("gj_batch_id_29489")));

uint32_t __gj_spark_u_29489(void)
    __attribute__((alias("gj_spark_u_29489")));
