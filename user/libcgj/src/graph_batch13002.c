/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13002: continuum product deepen lane id
 * (wave 30).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_13002(void);
 *     - Returns the soft continuum product-deepen lane identity for
 *       this continuum (always 13002). Soft compile-time lane tag;
 *       not a live scheduler or path probe.
 *   uint32_t __gj_continuum_product_deepen_lane_13002  (alias)
 *   __libcgj_batch13002_marker = "libcgj-batch13002"
 *
 * Exclusive continuum CREATE-ONLY (13001-13010: continuum product
 * deepen wave 30, post-13000). Unique gj_continuum_product_deepen_lane_13002
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_lane_12902,
 * gj_continuum_product_deepen_lane_12802, and
 * gj_continuum_product_deepen_root_13001. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13002_marker[] = "libcgj-batch13002";

/* Continuum product deepen lane identity (batch number). */
#define B13002_CPD_LANE  13002u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13002_lane(void)
{
	return B13002_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_13002 - continuum product deepen lane.
 *
 * Always returns 13002. Soft pure-data lane identity for the
 * 13001-13010 continuum (wave 30). Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_13002(void)
{
	(void)NULL;
	return b13002_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_13002(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_13002")));
