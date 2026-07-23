/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10302: continuum product deepen lane id
 * (wave 3).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_10302(void);
 *     - Returns the soft continuum product-deepen lane identity for
 *       this continuum (always 10302). Soft compile-time lane tag;
 *       not a live scheduler or path probe.
 *   uint32_t __gj_continuum_product_deepen_lane_10302  (alias)
 *   __libcgj_batch10302_marker = "libcgj-batch10302"
 *
 * Exclusive continuum CREATE-ONLY (10301-10310: continuum product
 * deepen wave 3). Unique gj_continuum_product_deepen_lane_10302
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_10301. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10302_marker[] = "libcgj-batch10302";

/* Continuum product deepen lane identity (batch number). */
#define B10302_CPD_LANE  10302u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10302_lane(void)
{
	return B10302_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_10302 - continuum product deepen lane.
 *
 * Always returns 10302. Soft pure-data lane identity for the
 * 10301-10310 continuum (wave 3). Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_10302(void)
{
	(void)NULL;
	return b10302_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_10302(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_10302")));
