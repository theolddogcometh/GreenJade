/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10202: continuum product deepen lane tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_10202(void);
 *     - Returns the soft continuum product-deepen lane identity tag for
 *       this continuum (always 10202). Compile-time lane id only; not a
 *       runtime scheduler or install-lane probe.
 *   uint32_t __gj_continuum_product_deepen_lane_10202  (alias)
 *   __libcgj_batch10202_marker = "libcgj-batch10202"
 *
 * Exclusive continuum CREATE-ONLY (10201-10210: post-10100 continuum
 * product deepen wave 2). Unique gj_continuum_product_deepen_lane_10202
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_10201 and prior continuum product
 * deepen surfaces. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10202_marker[] = "libcgj-batch10202";

/* Continuum product deepen lane identity (this batch). */
#define B10202_CPD_LANE  10202u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10202_lane(void)
{
	return B10202_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_10202 - continuum product deepen lane.
 *
 * Always returns 10202. Soft pure-data lane identity tag for the
 * 10201-10210 post-10100 continuum product deepen wave 2. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_10202(void)
{
	(void)NULL;
	return b10202_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_10202(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_10202")));
