/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14704: product readiness score tag (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14704(void);
 *     - Returns the compile-time graph batch number for this TU (14704).
 *   uint32_t gj_product_score_14704(void);
 *     - Returns 0 (product readiness score tag for the milestone 14725
 *       continuum). Soft compile-time product score tag; still open.
 *   uint32_t __gj_batch_id_14704  (alias)
 *   uint32_t __gj_product_score_14704  (alias)
 *   __libcgj_batch14704_marker = "libcgj-batch14704"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from gj_*_14700 / gj_*_14600 /
 * gj_*_14500 milestone surfaces and sibling 14725 milestone symbols.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14704_marker[] = "libcgj-batch14704";

/* Product score for wave 14725 (still open). */
#define B14704_ID             14704u
#define B14704_PRODUCT_SCORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14704_id(void)
{
	return B14704_ID;
}

static uint32_t
b14704_score(void)
{
	return B14704_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14704 - report this TU's graph batch number.
 *
 * Always returns 14704.
 */
uint32_t
gj_batch_id_14704(void)
{
	(void)NULL;
	return b14704_id();
}

/*
 * gj_product_score_14704 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_product_score_14704(void)
{
	return b14704_score();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14704(void)
    __attribute__((alias("gj_batch_id_14704")));

uint32_t __gj_product_score_14704(void)
    __attribute__((alias("gj_product_score_14704")));
