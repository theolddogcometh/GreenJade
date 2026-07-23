/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13902: continuum product deepen lane id
 * (wave 39).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_13902(void);
 *     - Returns the soft continuum product-deepen lane identity for
 *       this continuum (always 13902). Soft compile-time lane tag;
 *       not a live scheduler or path probe.
 *   uint32_t __gj_continuum_product_deepen_lane_13902  (alias)
 *   __libcgj_batch13902_marker = "libcgj-batch13902"
 *
 * Exclusive continuum CREATE-ONLY (13901-13910: continuum product
 * deepen wave 39, post-13900). Unique gj_continuum_product_deepen_lane_13902
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_lane_13702,
 * gj_continuum_product_deepen_lane_13602, and
 * gj_continuum_product_deepen_root_13901. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13902_marker[] = "libcgj-batch13902";

/* Continuum product deepen lane identity (batch number). */
#define B13902_CPD_LANE  13902u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13902_lane(void)
{
	return B13902_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_13902 - continuum product deepen lane.
 *
 * Always returns 13902. Soft pure-data lane identity for the
 * 13901-13910 continuum (wave 39). Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_13902(void)
{
	(void)NULL;
	return b13902_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_13902(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_13902")));
