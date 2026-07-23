/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6121: Mesa RADV feature-bit word width.
 *
 * Surface (unique symbols):
 *   uint32_t gj_radv_feat_word_bits_6121(void);
 *     - Returns 32 (soft RADV feature mask word width in bits).
 *   uint32_t __gj_radv_feat_word_bits_6121  (alias)
 *   __libcgj_batch6121_marker = "libcgj-batch6121"
 *
 * Exclusive CREATE-ONLY (6121-6130: mesa radv feature bit stubs).
 * Unique gj_radv_feat_word_bits_6121 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6121_marker[] = "libcgj-batch6121";

/* Soft: RADV feature mask is a uint32_t bitfield. */
#define B6121_WORD_BITS  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6121_word_bits(void)
{
	return B6121_WORD_BITS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_radv_feat_word_bits_6121 - Mesa RADV feature-bit word width.
 *
 * Always returns 32. Soft pure-data layout tag for a single uint32_t
 * feature mask word. Does not probe ICD or Vulkan. No parent wires.
 */
uint32_t
gj_radv_feat_word_bits_6121(void)
{
	(void)NULL;
	return b6121_word_bits();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_radv_feat_word_bits_6121(void)
    __attribute__((alias("gj_radv_feat_word_bits_6121")));
