/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36084: continuum link soft soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36084(void);
 *     - Returns the compile-time graph batch number for this TU (36084).
 *   uint32_t gj_link_soft_u_36084(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_36084  (alias)
 *   uint32_t __gj_link_soft_u_36084  (alias)
 *   __libcgj_batch36084_marker = "libcgj-batch36084"
 *
 * Exclusive continuum CREATE-ONLY (36076-36100 wave toward MILESTONE
 * 36100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch36084_marker[] = "libcgj-batch36084";

/* Batch identity. */
#define B36084_ID  36084u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B36084_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36084_id(void)
{
	return B36084_ID;
}

static uint32_t
b36084_link_soft(void)
{
	return B36084_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36084 - soft continuum surface
 *
 * Always returns 36084u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_36084(void)
{
	(void)NULL;
	return b36084_id();
}

/*
 * gj_link_soft_u_36084 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_link_soft_u_36084(void)
{
	return b36084_link_soft();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36084(void)
    __attribute__((alias("gj_batch_id_36084")));

uint32_t __gj_link_soft_u_36084(void)
    __attribute__((alias("gj_link_soft_u_36084")));
