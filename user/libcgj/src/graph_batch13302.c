/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13302: continuum product deepen lane id
 * (wave 33).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_13302(void);
 *     - Returns the soft continuum product-deepen lane identity for
 *       this continuum (always 13302). Soft compile-time lane tag;
 *       not a live scheduler or path probe.
 *   uint32_t __gj_continuum_product_deepen_lane_13302  (alias)
 *   __libcgj_batch13302_marker = "libcgj-batch13302"
 *
 * Exclusive continuum CREATE-ONLY (13301-13310: continuum product
 * deepen wave 33, post-13300). Unique gj_continuum_product_deepen_lane_13302
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_lane_13202,
 * gj_continuum_product_deepen_lane_13102, and
 * gj_continuum_product_deepen_root_13301. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13302_marker[] = "libcgj-batch13302";

/* Continuum product deepen lane identity (batch number). */
#define B13302_CPD_LANE  13302u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13302_lane(void)
{
	return B13302_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_13302 - continuum product deepen lane.
 *
 * Always returns 13302. Soft pure-data lane identity for the
 * 13301-13310 continuum (wave 33). Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_13302(void)
{
	(void)NULL;
	return b13302_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_13302(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_13302")));
