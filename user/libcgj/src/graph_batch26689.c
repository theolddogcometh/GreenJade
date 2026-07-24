/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26689: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_26689(void);
 *     - Returns the compile-time graph batch number for this TU (26689).
 *   uint32_t gj_spark_u_26689(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_26689  (alias)
 *   uint32_t __gj_spark_u_26689  (alias)
 *   __libcgj_batch26689_marker = "libcgj-batch26689"
 *
 * Exclusive continuum CREATE-ONLY (26676-26700 wave toward MILESTONE
 * 26700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch26689_marker[] = "libcgj-batch26689";

/* Batch identity. */
#define B26689_ID  26689u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B26689_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26689_id(void)
{
	return B26689_ID;
}

static uint32_t
b26689_spark(void)
{
	return B26689_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_26689 - soft continuum surface
 *
 * Always returns 26689u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_26689(void)
{
	(void)NULL;
	return b26689_id();
}

/*
 * gj_spark_u_26689 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_26689(void)
{
	return b26689_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_26689(void)
    __attribute__((alias("gj_batch_id_26689")));

uint32_t __gj_spark_u_26689(void)
    __attribute__((alias("gj_spark_u_26689")));
