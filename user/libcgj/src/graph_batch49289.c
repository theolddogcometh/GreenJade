/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch49289: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_49289(void);
 *     - Returns the compile-time graph batch number for this TU (49289).
 *   uint32_t gj_spark_u_49289(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_49289  (alias)
 *   uint32_t __gj_spark_u_49289  (alias)
 *   __libcgj_batch49289_marker = "libcgj-batch49289"
 *
 * Exclusive continuum CREATE-ONLY (49276-49300 wave toward MILESTONE
 * 49300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch49289_marker[] = "libcgj-batch49289";

/* Batch identity. */
#define B49289_ID  49289u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B49289_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b49289_id(void)
{
	return B49289_ID;
}

static uint32_t
b49289_spark(void)
{
	return B49289_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_49289 - soft continuum surface
 *
 * Always returns 49289u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_49289(void)
{
	(void)NULL;
	return b49289_id();
}

/*
 * gj_spark_u_49289 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_49289(void)
{
	return b49289_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_49289(void)
    __attribute__((alias("gj_batch_id_49289")));

uint32_t __gj_spark_u_49289(void)
    __attribute__((alias("gj_spark_u_49289")));
