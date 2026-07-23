/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13802: continuum product deepen lane id
 * (wave 38).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_13802(void);
 *     - Returns the soft continuum product-deepen lane identity for
 *       this continuum (always 13802). Soft compile-time lane tag;
 *       not a live scheduler or path probe.
 *   uint32_t __gj_continuum_product_deepen_lane_13802  (alias)
 *   __libcgj_batch13802_marker = "libcgj-batch13802"
 *
 * Exclusive continuum CREATE-ONLY (13801-13810: continuum product
 * deepen wave 38, post-13800). Unique gj_continuum_product_deepen_lane_13802
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_lane_13702,
 * gj_continuum_product_deepen_lane_13602, and
 * gj_continuum_product_deepen_root_13801. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13802_marker[] = "libcgj-batch13802";

/* Continuum product deepen lane identity (batch number). */
#define B13802_CPD_LANE  13802u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13802_lane(void)
{
	return B13802_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_13802 - continuum product deepen lane.
 *
 * Always returns 13802. Soft pure-data lane identity for the
 * 13801-13810 continuum (wave 38). Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_13802(void)
{
	(void)NULL;
	return b13802_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_13802(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_13802")));
