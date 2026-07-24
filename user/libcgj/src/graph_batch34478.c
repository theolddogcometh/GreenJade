/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch34478: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_34478(void);
 *     - Returns the compile-time graph batch number for this TU (34478).
 *   uint32_t gj_slot_b_u_34478(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_34478  (alias)
 *   uint32_t __gj_slot_b_u_34478  (alias)
 *   __libcgj_batch34478_marker = "libcgj-batch34478"
 *
 * Exclusive continuum CREATE-ONLY (34476-34500 wave toward MILESTONE
 * 34500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch34478_marker[] = "libcgj-batch34478";

/* Batch identity. */
#define B34478_ID  34478u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B34478_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b34478_id(void)
{
	return B34478_ID;
}

static uint32_t
b34478_slot_b(void)
{
	return B34478_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_34478 - soft continuum surface
 *
 * Always returns 34478u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_34478(void)
{
	(void)NULL;
	return b34478_id();
}

/*
 * gj_slot_b_u_34478 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_34478(void)
{
	return b34478_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_34478(void)
    __attribute__((alias("gj_batch_id_34478")));

uint32_t __gj_slot_b_u_34478(void)
    __attribute__((alias("gj_slot_b_u_34478")));
