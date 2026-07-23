/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14602: continuum product deepen lane id
 * (wave 46).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_lane_14602(void);
 *     - Returns 0 (continuum product-deepen lane soft baseline for
 *       wave 46). Soft compile-time lane tag; not a live scheduler
 *       or path probe.
 *   uint32_t __gj_continuum_product_deepen_lane_14602  (alias)
 *   __libcgj_batch14602_marker = "libcgj-batch14602"
 *
 * Exclusive continuum CREATE-ONLY (14601-14610: continuum product deepen wave 46, post-14600). Unique surface only; no multi-def. Distinct from prior continuum_product_deepen / continuum_smoke_soft / continuum_dyn_soft symbols (1450x/1440x/…). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14602_marker[] = "libcgj-batch14602";

/* Continuum product deepen lane soft baseline. */
#define B14602_CPD_LANE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14602_lane(void)
{
	return B14602_CPD_LANE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_lane_14602 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_lane_14602(void)
{
	(void)NULL;
	return b14602_lane();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_lane_14602(void)
    __attribute__((alias("gj_continuum_product_deepen_lane_14602")));
