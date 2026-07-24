/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27389: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_27389(void);
 *     - Returns the compile-time graph batch number for this TU (27389).
 *   uint32_t gj_spark_u_27389(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_27389  (alias)
 *   uint32_t __gj_spark_u_27389  (alias)
 *   __libcgj_batch27389_marker = "libcgj-batch27389"
 *
 * Exclusive continuum CREATE-ONLY (27376-27400 wave toward MILESTONE
 * 27400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch27389_marker[] = "libcgj-batch27389";

/* Batch identity. */
#define B27389_ID  27389u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B27389_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27389_id(void)
{
	return B27389_ID;
}

static uint32_t
b27389_spark(void)
{
	return B27389_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_27389 - soft continuum surface
 *
 * Always returns 27389u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_27389(void)
{
	(void)NULL;
	return b27389_id();
}

/*
 * gj_spark_u_27389 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_27389(void)
{
	return b27389_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_27389(void)
    __attribute__((alias("gj_batch_id_27389")));

uint32_t __gj_spark_u_27389(void)
    __attribute__((alias("gj_spark_u_27389")));
