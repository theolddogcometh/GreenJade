/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5777: ext4 soft feature-bit pack / extract.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ext4_feature_pack_5777(uint32_t extents, uint32_t journal,
 *                                     uint32_t dir_index, uint32_t flex64);
 *     - Pack soft feature flags:
 *         bit0 = extents (0/1), bit1 = journal (0/1),
 *         bit2 = dir_index (0/1), bit3 = 64bit (from flex64 low),
 *         bit4 = flex_bg (from flex64 bit1).
 *   uint32_t gj_ext4_feature_extents_5777(uint32_t packed);
 *   uint32_t gj_ext4_feature_journal_5777(uint32_t packed);
 *   uint32_t gj_ext4_feature_dir_index_5777(uint32_t packed);
 *   uint32_t gj_batch_id_5777(void);
 *   __ aliases for each public symbol
 *   __libcgj_batch5777_marker = "libcgj-batch5777"
 *
 * Exclusive continuum CREATE-ONLY (5771-5780). Unique *_5777 surfaces
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5777_marker[] = "libcgj-batch5777";

#define B5777_BATCH_ID  5777u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5777_bit01(uint32_t u32V)
{
	return (u32V != 0u) ? 1u : 0u;
}

static uint32_t
b5777_pack(uint32_t u32Extents, uint32_t u32Journal, uint32_t u32DirIndex,
    uint32_t u32Flex64)
{
	uint32_t u32P;

	u32P = b5777_bit01(u32Extents)
	    | (b5777_bit01(u32Journal) << 1)
	    | (b5777_bit01(u32DirIndex) << 2)
	    | ((u32Flex64 & 1u) << 3)   /* 64bit */
	    | (((u32Flex64 >> 1) & 1u) << 4); /* flex_bg */
	return u32P;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ext4_feature_pack_5777 - pack soft ext4 feature flags.
 *
 * extents/journal/dir_index: non-zero => set corresponding bit
 * flex64: bit0 => 64bit, bit1 => flex_bg
 *
 * Returns packed 5-bit soft feature word.
 */
uint32_t
gj_ext4_feature_pack_5777(uint32_t extents, uint32_t journal,
    uint32_t dir_index, uint32_t flex64)
{
	(void)NULL;
	return b5777_pack(extents, journal, dir_index, flex64);
}

uint32_t
gj_ext4_feature_extents_5777(uint32_t packed)
{
	return packed & 1u;
}

uint32_t
gj_ext4_feature_journal_5777(uint32_t packed)
{
	return (packed >> 1) & 1u;
}

uint32_t
gj_ext4_feature_dir_index_5777(uint32_t packed)
{
	return (packed >> 2) & 1u;
}

/*
 * gj_batch_id_5777 - report this TU's graph batch number.
 *
 * Always returns 5777.
 */
uint32_t
gj_batch_id_5777(void)
{
	return B5777_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_ext4_feature_pack_5777(uint32_t extents, uint32_t journal,
    uint32_t dir_index, uint32_t flex64)
    __attribute__((alias("gj_ext4_feature_pack_5777")));

uint32_t __gj_ext4_feature_extents_5777(uint32_t packed)
    __attribute__((alias("gj_ext4_feature_extents_5777")));

uint32_t __gj_ext4_feature_journal_5777(uint32_t packed)
    __attribute__((alias("gj_ext4_feature_journal_5777")));

uint32_t __gj_ext4_feature_dir_index_5777(uint32_t packed)
    __attribute__((alias("gj_ext4_feature_dir_index_5777")));

uint32_t __gj_batch_id_5777(void)
    __attribute__((alias("gj_batch_id_5777")));
