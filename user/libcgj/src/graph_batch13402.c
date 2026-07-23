/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13402: continuum product deepen lane id
 * (wave 34).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_13402(void);
 *     - Returns the soft continuum product-deepen lane identity for
 *       this continuum (always 13402). Soft compile-time lane tag;
 *       not a live scheduler or path probe.
 *   uint32_t __gj_continuum_product_deepen_lane_13402  (alias)
 *   __libcgj_batch13402_marker = "libcgj-batch13402"
 *
 * Exclusive continuum CREATE-ONLY (13401-13410: continuum product
 * deepen wave 34, post-13400). Unique gj_continuum_product_deepen_lane_13402
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_lane_13302,
 * gj_continuum_product_deepen_lane_13202, and
 * gj_continuum_product_deepen_root_13401. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13402_marker[] = "libcgj-batch13402";

/* Continuum product deepen lane identity (batch number). */
#define B13402_CPD_LANE  13402u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13402_lane(void)
{
	return B13402_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_13402 - continuum product deepen lane.
 *
 * Always returns 13402. Soft pure-data lane identity for the
 * 13401-13410 continuum (wave 34). Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_13402(void)
{
	(void)NULL;
	return b13402_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_13402(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_13402")));
