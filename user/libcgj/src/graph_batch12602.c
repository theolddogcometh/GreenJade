/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12602: continuum product deepen lane id
 * (wave 26).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_12602(void);
 *     - Returns the soft continuum product-deepen lane identity for
 *       this continuum (always 12602). Soft compile-time lane tag;
 *       not a live scheduler or path probe.
 *   uint32_t __gj_continuum_product_deepen_lane_12602  (alias)
 *   __libcgj_batch12602_marker = "libcgj-batch12602"
 *
 * Exclusive continuum CREATE-ONLY (12601-12610: continuum product
 * deepen wave 26, post-12600). Unique gj_continuum_product_deepen_lane_12602
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_lane_12502,
 * gj_continuum_product_deepen_lane_12402, and
 * gj_continuum_product_deepen_root_12601. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12602_marker[] = "libcgj-batch12602";

/* Continuum product deepen lane identity (batch number). */
#define B12602_CPD_LANE  12602u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12602_lane(void)
{
	return B12602_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_12602 - continuum product deepen lane.
 *
 * Always returns 12602. Soft pure-data lane identity for the
 * 12601-12610 continuum (wave 26). Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_12602(void)
{
	(void)NULL;
	return b12602_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_12602(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_12602")));
