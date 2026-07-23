/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6104: product install deepen step codes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_install_deepen_step_6104(uint32_t step_id);
 *     - Map product install-deepen step id (0..5) to a stable step
 *       code. Unknown step_id → 0.
 *   uint32_t gj_product_install_deepen_step_count_6104(void);
 *     - Number of known product install-deepen steps (always 6).
 *   uint32_t __gj_product_install_deepen_step_6104  (alias)
 *   uint32_t __gj_product_install_deepen_step_count_6104  (alias)
 *   __libcgj_batch6104_marker = "libcgj-batch6104"
 *
 * Exclusive continuum CREATE-ONLY (6101-6110: post-6100 product
 * deepen install). Unique *_6104 surfaces only; no multi-def.
 * Distinct from gj_product_media_step_5804 and
 * gj_product_bootstrap_step_5814. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6104_marker[] = "libcgj-batch6104";

#define B6104_STEP_MAX  6u

/*
 * Product install-deepen step codes:
 *   0 inventory, 1 deepen mask, 2 path slot, 3 image bind,
 *   4 seal prep, 5 product seal.
 */
static const uint16_t s_b6104_step[B6104_STEP_MAX] = {
	0x494Eu, /* 0 IN inventory */
	0x444Du, /* 1 DM deepen mask */
	0x5053u, /* 2 PS path slot */
	0x4942u, /* 3 IB image bind */
	0x5350u, /* 4 SP seal prep */
	0x5045u  /* 5 PE product seal (PE = product end/seal) */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6104_step(uint32_t u32Step)
{
	if (u32Step >= B6104_STEP_MAX) {
		return 0u;
	}
	return (uint32_t)s_b6104_step[u32Step];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_install_deepen_step_6104 - map deepen step id to code.
 *
 * step_id: 0..5 known; else 0. Soft table only; does not write images.
 * No parent wires.
 */
uint32_t
gj_product_install_deepen_step_6104(uint32_t step_id)
{
	(void)NULL;
	return b6104_step(step_id);
}

/*
 * gj_product_install_deepen_step_count_6104 - known deepen step count.
 *
 * Always returns 6.
 */
uint32_t
gj_product_install_deepen_step_count_6104(void)
{
	return B6104_STEP_MAX;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_product_install_deepen_step_6104(uint32_t step_id)
    __attribute__((alias("gj_product_install_deepen_step_6104")));

uint32_t __gj_product_install_deepen_step_count_6104(void)
    __attribute__((alias("gj_product_install_deepen_step_count_6104")));
