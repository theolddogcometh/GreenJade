/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14002: continuum product deepen lane id
 * (wave 40).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_14002(void);
 *     - Returns 0 (continuum product-deepen lane soft baseline for
 *       wave 40). Soft compile-time lane tag; not a live scheduler
 *       or path probe.
 *   uint32_t __gj_continuum_product_deepen_lane_14002  (alias)
 *   __libcgj_batch14002_marker = "libcgj-batch14002"
 *
 * Exclusive continuum CREATE-ONLY (14001-14010: continuum product
 * deepen wave 40, post-14000). Unique gj_continuum_product_deepen_lane_14002
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_lane_13002,
 * gj_continuum_product_deepen_lane_12902, and
 * gj_continuum_product_deepen_root_14001. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14002_marker[] = "libcgj-batch14002";

/* Continuum product deepen lane soft baseline. */
#define B14002_CPD_LANE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14002_lane(void)
{
	return B14002_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_14002 - continuum product deepen lane.
 *
 * Always returns 0. Soft pure-data lane tag for the 14001-14010
 * continuum (wave 40). Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_14002(void)
{
	(void)NULL;
	return b14002_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_14002(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_14002")));
