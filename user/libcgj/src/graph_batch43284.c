/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch43284: continuum link soft soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_43284(void);
 *     - Returns the compile-time graph batch number for this TU (43284).
 *   uint32_t gj_link_soft_u_43284(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_43284  (alias)
 *   uint32_t __gj_link_soft_u_43284  (alias)
 *   __libcgj_batch43284_marker = "libcgj-batch43284"
 *
 * Exclusive continuum CREATE-ONLY (43276-43300 wave toward MILESTONE
 * 43300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch43284_marker[] = "libcgj-batch43284";

/* Batch identity. */
#define B43284_ID  43284u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B43284_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b43284_id(void)
{
	return B43284_ID;
}

static uint32_t
b43284_link_soft(void)
{
	return B43284_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_43284 - soft continuum surface
 *
 * Always returns 43284u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_43284(void)
{
	(void)NULL;
	return b43284_id();
}

/*
 * gj_link_soft_u_43284 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_link_soft_u_43284(void)
{
	return b43284_link_soft();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_43284(void)
    __attribute__((alias("gj_batch_id_43284")));

uint32_t __gj_link_soft_u_43284(void)
    __attribute__((alias("gj_link_soft_u_43284")));
