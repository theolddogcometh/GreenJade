/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12002: continuum product deepen lane id
 * (wave 20).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_12002(void);
 *     - Returns the soft continuum product-deepen lane identity for
 *       this continuum (always 12002). Soft compile-time lane tag;
 *       not a live scheduler or path probe.
 *   uint32_t __gj_continuum_product_deepen_lane_12002  (alias)
 *   __libcgj_batch12002_marker = "libcgj-batch12002"
 *
 * Exclusive continuum CREATE-ONLY (12001-12010: continuum product
 * deepen wave 20, post-12000). Unique gj_continuum_product_deepen_lane_12002
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_lane_11902,
 * gj_continuum_product_deepen_lane_11802, and
 * gj_continuum_product_deepen_root_12001. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12002_marker[] = "libcgj-batch12002";

/* Continuum product deepen lane identity (batch number). */
#define B12002_CPD_LANE  12002u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12002_lane(void)
{
	return B12002_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_12002 - continuum product deepen lane.
 *
 * Always returns 12002. Soft pure-data lane identity for the
 * 12001-12010 continuum (wave 20). Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_12002(void)
{
	(void)NULL;
	return b12002_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_12002(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_12002")));
