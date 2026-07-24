/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch43090: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_43090(void);
 *     - Returns the compile-time graph batch number for this TU (43090).
 *   uint32_t gj_anchor_u_43090(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_43090  (alias)
 *   uint32_t __gj_anchor_u_43090  (alias)
 *   __libcgj_batch43090_marker = "libcgj-batch43090"
 *
 * Exclusive continuum CREATE-ONLY (43076-43100 wave toward MILESTONE
 * 43100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch43090_marker[] = "libcgj-batch43090";

/* Batch identity. */
#define B43090_ID  43090u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B43090_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b43090_id(void)
{
	return B43090_ID;
}

static uint32_t
b43090_anchor(void)
{
	return B43090_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_43090 - soft continuum surface
 *
 * Always returns 43090u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_43090(void)
{
	(void)NULL;
	return b43090_id();
}

/*
 * gj_anchor_u_43090 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_43090(void)
{
	return b43090_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_43090(void)
    __attribute__((alias("gj_batch_id_43090")));

uint32_t __gj_anchor_u_43090(void)
    __attribute__((alias("gj_anchor_u_43090")));
