/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12502: continuum product deepen lane id
 * (wave 25).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_12502(void);
 *     - Returns the soft continuum product-deepen lane identity for
 *       this continuum (always 12502). Soft compile-time lane tag;
 *       not a live scheduler or path probe.
 *   uint32_t __gj_continuum_product_deepen_lane_12502  (alias)
 *   __libcgj_batch12502_marker = "libcgj-batch12502"
 *
 * Exclusive continuum CREATE-ONLY (12501-12510: continuum product
 * deepen wave 25, post-12500). Unique gj_continuum_product_deepen_lane_12502
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_lane_12402,
 * gj_continuum_product_deepen_lane_12302, and
 * gj_continuum_product_deepen_root_12501. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12502_marker[] = "libcgj-batch12502";

/* Continuum product deepen lane identity (batch number). */
#define B12502_CPD_LANE  12502u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12502_lane(void)
{
	return B12502_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_12502 - continuum product deepen lane.
 *
 * Always returns 12502. Soft pure-data lane identity for the
 * 12501-12510 continuum (wave 25). Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_12502(void)
{
	(void)NULL;
	return b12502_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_12502(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_12502")));
