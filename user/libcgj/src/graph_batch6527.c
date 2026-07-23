/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6527: Top50 title deepen stage-ready popcount.
 *
 * Surface (unique symbols):
 *   uint32_t gj_top50_title_stage_count_6527(uint32_t mask);
 *     - Count set bits among the four title-deepen stages
 *       (rank/appid/result/owner: bits 0..3), range [0, 4]. Soft
 *       pure-data only.
 *   uint32_t __gj_top50_title_stage_count_6527  (alias)
 *   __libcgj_batch6527_marker = "libcgj-batch6527"
 *
 * Exclusive continuum CREATE-ONLY (6521-6530: post-6500 top50 title
 * deepen). Unique gj_top50_title_stage_count_6527 surface only; no
 * multi-def. Distinct from gj_top50_path_stage_count_6027. Title stages:
 * bit0=rank, bit1=appid, bit2=result, bit3=owner. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6527_marker[] = "libcgj-batch6527";

/* rank | appid | result | owner (bits 0..3). */
#define B6527_STAGE_MASK  0xfu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6527_stage_count(uint32_t uMask)
{
	uint32_t uBits;
	uint32_t uN;

	uBits = uMask & B6527_STAGE_MASK;
	uN = 0u;
	while (uBits != 0u) {
		uN += uBits & 1u;
		uBits >>= 1;
	}
	return uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_top50_title_stage_count_6527 - count ready Top50 title deepen stages.
 *
 * mask: product Top50 title bitmask (low 4 bits = stages).
 * Returns number of set stage bits in [0, 4].
 * Soft pure-data only; does not call libc. No parent wires.
 */
uint32_t
gj_top50_title_stage_count_6527(uint32_t uMask)
{
	(void)NULL;
	return b6527_stage_count(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_top50_title_stage_count_6527(uint32_t uMask)
    __attribute__((alias("gj_top50_title_stage_count_6527")));
