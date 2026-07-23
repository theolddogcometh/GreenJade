/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5814: product bootstrap step codes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_bootstrap_step_5814(uint32_t step_id);
 *     - Map product bootstrap step id (0..5) to a stable step code.
 *       Unknown step_id → 0.
 *   uint32_t gj_product_bootstrap_step_count_5814(void);
 *     - Number of known product bootstrap steps (always 6).
 *   uint32_t __gj_product_bootstrap_step_5814  (alias)
 *   uint32_t __gj_product_bootstrap_step_count_5814  (alias)
 *   __libcgj_batch5814_marker = "libcgj-batch5814"
 *
 * Exclusive continuum CREATE-ONLY (5811-5820: product bar3 steam
 * bootstrap finalize). Unique *_5814 surfaces only; no multi-def.
 * Distinct from gj_bar3_steam_boot_phase_5812. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5814_marker[] = "libcgj-batch5814";

#define B5814_STEP_MAX  6u

/*
 * Product bootstrap step codes:
 *   0 inventory, 1 bar3 mask, 2 steam slot, 3 bootstrap run,
 *   4 finalize prep, 5 product seal.
 */
static const uint16_t s_b5814_step[B5814_STEP_MAX] = {
	0x494eu, /* 0 IN inventory */
	0x4233u, /* 1 B3 bar3 mask */
	0x5353u, /* 2 SS steam slot */
	0x4252u, /* 3 BR bootstrap run */
	0x4650u, /* 4 FP finalize prep */
	0x5053u  /* 5 PS product seal */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5814_step(uint32_t u32Step)
{
	if (u32Step >= B5814_STEP_MAX) {
		return 0u;
	}
	return (uint32_t)s_b5814_step[u32Step];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_bootstrap_step_5814 - map product bootstrap step to code.
 *
 * step_id: 0..5 known; else 0. Soft table only; does not run bootstrap.
 * No parent wires.
 */
uint32_t
gj_product_bootstrap_step_5814(uint32_t step_id)
{
	(void)NULL;
	return b5814_step(step_id);
}

/*
 * gj_product_bootstrap_step_count_5814 - known product bootstrap steps.
 *
 * Always returns 6.
 */
uint32_t
gj_product_bootstrap_step_count_5814(void)
{
	return B5814_STEP_MAX;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_product_bootstrap_step_5814(uint32_t step_id)
    __attribute__((alias("gj_product_bootstrap_step_5814")));

uint32_t __gj_product_bootstrap_step_count_5814(void)
    __attribute__((alias("gj_product_bootstrap_step_count_5814")));
