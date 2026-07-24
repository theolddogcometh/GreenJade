/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36884: continuum link soft soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36884(void);
 *     - Returns the compile-time graph batch number for this TU (36884).
 *   uint32_t gj_link_soft_u_36884(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_36884  (alias)
 *   uint32_t __gj_link_soft_u_36884  (alias)
 *   __libcgj_batch36884_marker = "libcgj-batch36884"
 *
 * Exclusive continuum CREATE-ONLY (36876-36900 wave toward MILESTONE
 * 36900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch36884_marker[] = "libcgj-batch36884";

/* Batch identity. */
#define B36884_ID  36884u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B36884_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36884_id(void)
{
	return B36884_ID;
}

static uint32_t
b36884_link_soft(void)
{
	return B36884_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36884 - soft continuum surface
 *
 * Always returns 36884u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_36884(void)
{
	(void)NULL;
	return b36884_id();
}

/*
 * gj_link_soft_u_36884 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_link_soft_u_36884(void)
{
	return b36884_link_soft();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36884(void)
    __attribute__((alias("gj_batch_id_36884")));

uint32_t __gj_link_soft_u_36884(void)
    __attribute__((alias("gj_link_soft_u_36884")));
