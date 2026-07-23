/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5804: product media step codes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_media_step_5804(uint32_t step_id);
 *     - Map product install-media step id (0..5) to a stable step code.
 *       Unknown step_id → 0.
 *   uint32_t gj_product_media_step_count_5804(void);
 *     - Number of known product media steps (always 6).
 *   uint32_t __gj_product_media_step_5804  (alias)
 *   uint32_t __gj_product_media_step_count_5804  (alias)
 *   __libcgj_batch5804_marker = "libcgj-batch5804"
 *
 * Exclusive continuum CREATE-ONLY (5801-5810: product bar3 install
 * media finalize). Unique *_5804 surfaces only; no multi-def.
 * Distinct from gj_bar3_install_media_phase_5802 and
 * gj_product_bootstrap_step_5814. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5804_marker[] = "libcgj-batch5804";

#define B5804_STEP_MAX  6u

/*
 * Product install-media step codes:
 *   0 inventory, 1 bar3 mask, 2 media slot, 3 install image,
 *   4 finalize prep, 5 product seal.
 */
static const uint16_t s_b5804_step[B5804_STEP_MAX] = {
	0x494Eu, /* 0 IN inventory */
	0x4233u, /* 1 B3 bar3 mask */
	0x4D53u, /* 2 MS media slot */
	0x4949u, /* 3 II install image */
	0x4650u, /* 4 FP finalize prep */
	0x5053u  /* 5 PS product seal */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5804_step(uint32_t u32Step)
{
	if (u32Step >= B5804_STEP_MAX) {
		return 0u;
	}
	return (uint32_t)s_b5804_step[u32Step];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_media_step_5804 - map product media step id to step code.
 *
 * step_id: 0..5 known; else 0. Soft table only; does not write images.
 * No parent wires.
 */
uint32_t
gj_product_media_step_5804(uint32_t step_id)
{
	(void)NULL;
	return b5804_step(step_id);
}

/*
 * gj_product_media_step_count_5804 - known product media step count.
 *
 * Always returns 6.
 */
uint32_t
gj_product_media_step_count_5804(void)
{
	return B5804_STEP_MAX;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_product_media_step_5804(uint32_t step_id)
    __attribute__((alias("gj_product_media_step_5804")));

uint32_t __gj_product_media_step_count_5804(void)
    __attribute__((alias("gj_product_media_step_count_5804")));
