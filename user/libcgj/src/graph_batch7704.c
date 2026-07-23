/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7704: bar3 product deepen step codes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_product_deepen_step_7704(uint32_t step_id);
 *     - Map bar3 product-deepen step id (0..5) to a stable step code.
 *       Unknown step_id → 0.
 *   uint32_t gj_bar3_product_deepen_step_count_7704(void);
 *     - Number of known bar3 product-deepen steps (always 6).
 *   uint32_t __gj_bar3_product_deepen_step_7704  (alias)
 *   uint32_t __gj_bar3_product_deepen_step_count_7704  (alias)
 *   __libcgj_batch7704_marker = "libcgj-batch7704"
 *
 * Exclusive continuum CREATE-ONLY (7701-7710: post-7700 bar3 product
 * deepen). Unique *_7704 surfaces only; no multi-def. Distinct from
 * gj_bar3_product_deepen_step_6704,
 * gj_continuum_product_deepen_step_7004, and
 * gj_product_install_deepen_step_6104. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7704_marker[] = "libcgj-batch7704";

#define B7704_STEP_MAX  6u

/*
 * Bar3 product-deepen step codes:
 *   0 inventory, 1 deepen mask, 2 bar3 slot, 3 product bind,
 *   4 seal prep, 5 bar3 seal.
 */
static const uint16_t s_b7704_step[B7704_STEP_MAX] = {
	0x494Eu, /* 0 IN inventory */
	0x444Du, /* 1 DM deepen mask */
	0x4253u, /* 2 BS bar3 slot */
	0x5042u, /* 3 PB product bind */
	0x5350u, /* 4 SP seal prep */
	0x4255u  /* 5 BU bar3 unit/seal */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7704_step(uint32_t u32Step)
{
	if (u32Step >= B7704_STEP_MAX) {
		return 0u;
	}
	return (uint32_t)s_b7704_step[u32Step];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_product_deepen_step_7704 - map BPD step id to code.
 *
 * step_id: 0..5 known; else 0. Soft table only; does not write images.
 * No parent wires.
 */
uint32_t
gj_bar3_product_deepen_step_7704(uint32_t step_id)
{
	(void)NULL;
	return b7704_step(step_id);
}

/*
 * gj_bar3_product_deepen_step_count_7704 - known BPD step count.
 *
 * Always returns 6.
 */
uint32_t
gj_bar3_product_deepen_step_count_7704(void)
{
	return B7704_STEP_MAX;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_bar3_product_deepen_step_7704(uint32_t step_id)
    __attribute__((alias("gj_bar3_product_deepen_step_7704")));

uint32_t __gj_bar3_product_deepen_step_count_7704(void)
    __attribute__((alias("gj_bar3_product_deepen_step_count_7704")));
