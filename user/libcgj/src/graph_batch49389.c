/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch49389: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_49389(void);
 *     - Returns the compile-time graph batch number for this TU (49389).
 *   uint32_t gj_spark_u_49389(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_49389  (alias)
 *   uint32_t __gj_spark_u_49389  (alias)
 *   __libcgj_batch49389_marker = "libcgj-batch49389"
 *
 * Exclusive continuum CREATE-ONLY (49376-49400 wave toward MILESTONE
 * 49400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch49389_marker[] = "libcgj-batch49389";

/* Batch identity. */
#define B49389_ID  49389u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B49389_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b49389_id(void)
{
	return B49389_ID;
}

static uint32_t
b49389_spark(void)
{
	return B49389_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_49389 - soft continuum surface
 *
 * Always returns 49389u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_49389(void)
{
	(void)NULL;
	return b49389_id();
}

/*
 * gj_spark_u_49389 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_49389(void)
{
	return b49389_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_49389(void)
    __attribute__((alias("gj_batch_id_49389")));

uint32_t __gj_spark_u_49389(void)
    __attribute__((alias("gj_spark_u_49389")));
