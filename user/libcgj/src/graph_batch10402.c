/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10402: continuum product deepen lane id
 * (wave 4).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_10402(void);
 *     - Returns the soft continuum product-deepen lane identity for
 *       this continuum (always 10402). Soft compile-time lane tag;
 *       not a live scheduler or path probe.
 *   uint32_t __gj_continuum_product_deepen_lane_10402  (alias)
 *   __libcgj_batch10402_marker = "libcgj-batch10402"
 *
 * Exclusive continuum CREATE-ONLY (10401-10410: continuum product
 * deepen wave 4). Unique gj_continuum_product_deepen_lane_10402
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_lane_10302,
 * gj_continuum_product_deepen_lane_10202, and
 * gj_continuum_product_deepen_root_10401. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10402_marker[] = "libcgj-batch10402";

/* Continuum product deepen lane identity (batch number). */
#define B10402_CPD_LANE  10402u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10402_lane(void)
{
	return B10402_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_10402 - continuum product deepen lane.
 *
 * Always returns 10402. Soft pure-data lane identity for the
 * 10401-10410 continuum (wave 4). Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_10402(void)
{
	(void)NULL;
	return b10402_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_10402(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_10402")));
