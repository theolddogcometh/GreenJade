/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5776: ext4 soft feature-bit mask / has stubs.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ext4_feature_mask_5776(void);
 *     - Soft known-feature mask (extents|journal|dir_index|64bit|flex_bg).
 *   uint32_t gj_ext4_feature_has_5776(uint32_t flags, uint32_t bit);
 *     - Return 1 when flags has the single bit set, else 0.
 *   uint32_t gj_ext4_feature_any_5776(uint32_t flags);
 *     - Return 1 when flags intersects the soft known mask, else 0.
 *   uint32_t gj_batch_id_5776(void);
 *   __ aliases for each public symbol
 *   __libcgj_batch5776_marker = "libcgj-batch5776"
 *
 * Soft layout only (not a real s_feature_incompat read). Exclusive
 * continuum CREATE-ONLY (5771-5780). Unique *_5776 surfaces only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5776_marker[] = "libcgj-batch5776";

#define B5776_BATCH_ID  5776u

/* Soft ext4-like feature bits (layout probe constants only). */
#define B5776_EXTENTS    0x01u
#define B5776_JOURNAL    0x02u
#define B5776_DIR_INDEX  0x04u
#define B5776_64BIT      0x08u
#define B5776_FLEX_BG    0x10u
#define B5776_MASK \
	(B5776_EXTENTS | B5776_JOURNAL | B5776_DIR_INDEX | \
	 B5776_64BIT | B5776_FLEX_BG)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5776_mask(void)
{
	return B5776_MASK;
}

static uint32_t
b5776_has(uint32_t u32Flags, uint32_t u32Bit)
{
	/* Require a single-bit probe; multi-bit bit arg still AND-tested. */
	return ((u32Flags & u32Bit) != 0u) ? 1u : 0u;
}

static uint32_t
b5776_any(uint32_t u32Flags)
{
	return ((u32Flags & B5776_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ext4_feature_mask_5776 - soft known ext4 feature mask.
 *
 * Always returns 0x1f (extents|journal|dir_index|64bit|flex_bg).
 * Pure product-path constant; not a superblock parse.
 */
uint32_t
gj_ext4_feature_mask_5776(void)
{
	(void)NULL;
	return b5776_mask();
}

/*
 * gj_ext4_feature_has_5776 - test one soft feature bit in flags.
 *
 * flags: candidate feature word
 * bit:   feature bit to test (e.g. 0x01 extents)
 *
 * Returns 1 if (flags & bit) != 0, else 0.
 */
uint32_t
gj_ext4_feature_has_5776(uint32_t flags, uint32_t bit)
{
	return b5776_has(flags, bit);
}

/*
 * gj_ext4_feature_any_5776 - any known soft feature bit set?
 *
 * flags: candidate feature word
 *
 * Returns 1 if flags intersects the soft known mask, else 0.
 */
uint32_t
gj_ext4_feature_any_5776(uint32_t flags)
{
	return b5776_any(flags);
}

/*
 * gj_batch_id_5776 - report this TU's graph batch number.
 *
 * Always returns 5776.
 */
uint32_t
gj_batch_id_5776(void)
{
	return B5776_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_ext4_feature_mask_5776(void)
    __attribute__((alias("gj_ext4_feature_mask_5776")));

uint32_t __gj_ext4_feature_has_5776(uint32_t flags, uint32_t bit)
    __attribute__((alias("gj_ext4_feature_has_5776")));

uint32_t __gj_ext4_feature_any_5776(uint32_t flags)
    __attribute__((alias("gj_ext4_feature_any_5776")));

uint32_t __gj_batch_id_5776(void)
    __attribute__((alias("gj_batch_id_5776")));
