/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch42289: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_42289(void);
 *     - Returns the compile-time graph batch number for this TU (42289).
 *   uint32_t gj_spark_u_42289(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_42289  (alias)
 *   uint32_t __gj_spark_u_42289  (alias)
 *   __libcgj_batch42289_marker = "libcgj-batch42289"
 *
 * Exclusive continuum CREATE-ONLY (42276-42300 wave toward MILESTONE
 * 42300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch42289_marker[] = "libcgj-batch42289";

/* Batch identity. */
#define B42289_ID  42289u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B42289_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b42289_id(void)
{
	return B42289_ID;
}

static uint32_t
b42289_spark(void)
{
	return B42289_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_42289 - soft continuum surface
 *
 * Always returns 42289u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_42289(void)
{
	(void)NULL;
	return b42289_id();
}

/*
 * gj_spark_u_42289 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_42289(void)
{
	return b42289_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_42289(void)
    __attribute__((alias("gj_batch_id_42289")));

uint32_t __gj_spark_u_42289(void)
    __attribute__((alias("gj_spark_u_42289")));
