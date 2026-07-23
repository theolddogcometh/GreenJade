/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5654: locale BCP-47-ish tag fingerprint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_tag_hash_5654(uint32_t c0, uint32_t c1,
 *                                    uint32_t c2, uint32_t c3);
 *     - Fold four 7-bit char codes (lang/region bytes) into a 32-bit
 *       FNV-ish tag id used by first-boot locale UI stubs.
 *   int gj_locale_tag_valid_5654(uint32_t tag);
 *     - 1 if tag has non-zero low 16 bits and high nibble mix matches
 *       fabric (soft structural check), else 0.
 *   uint32_t gj_locale_tag_lang16_5654(uint32_t tag);
 *     - Low 16 bits of tag (language half).
 *   uint32_t gj_batch_id_5654(void);
 *   __ aliases
 *   __libcgj_batch5654_marker = "libcgj-batch5654"
 *
 * Exclusive continuum CREATE-ONLY (5651-5660: post-install first-boot
 * wizard stubs, locale, keyboard, network setup flags). Unique *_5654
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5654_marker[] = "libcgj-batch5654";

#define B5654_BATCH_ID 5654u
#define B5654_FNV_OFF  2166136261u
#define B5654_FNV_PRIME 16777619u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5654_ascii7(uint32_t u32C)
{
	return u32C & 0x7fu;
}

static uint32_t
b5654_hash(uint32_t u32C0, uint32_t u32C1, uint32_t u32C2, uint32_t u32C3)
{
	uint32_t u32H;

	u32H = B5654_FNV_OFF ^ B5654_BATCH_ID;
	u32H ^= b5654_ascii7(u32C0);
	u32H *= B5654_FNV_PRIME;
	u32H ^= b5654_ascii7(u32C1);
	u32H *= B5654_FNV_PRIME;
	u32H ^= b5654_ascii7(u32C2);
	u32H *= B5654_FNV_PRIME;
	u32H ^= b5654_ascii7(u32C3);
	u32H *= B5654_FNV_PRIME;
	return u32H;
}

static int
b5654_valid(uint32_t u32Tag)
{
	uint32_t u32Lo;
	uint32_t u32Mix;

	u32Lo = u32Tag & 0xffffu;
	if (u32Lo == 0u) {
		return 0;
	}
	/* Soft fabric: high nibble of high byte should be non-zero-ish. */
	u32Mix = (u32Tag >> 28) & 0xfu;
	return (u32Mix != 0u || ((u32Tag >> 16) & 0xfffu) != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

uint32_t
gj_locale_tag_hash_5654(uint32_t c0, uint32_t c1, uint32_t c2, uint32_t c3)
{
	(void)NULL;
	return b5654_hash(c0, c1, c2, c3);
}

int
gj_locale_tag_valid_5654(uint32_t tag)
{
	return b5654_valid(tag);
}

uint32_t
gj_locale_tag_lang16_5654(uint32_t tag)
{
	return tag & 0xffffu;
}

uint32_t
gj_batch_id_5654(void)
{
	return B5654_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_locale_tag_hash_5654(uint32_t c0, uint32_t c1, uint32_t c2,
    uint32_t c3)
    __attribute__((alias("gj_locale_tag_hash_5654")));

int __gj_locale_tag_valid_5654(uint32_t tag)
    __attribute__((alias("gj_locale_tag_valid_5654")));

uint32_t __gj_locale_tag_lang16_5654(uint32_t tag)
    __attribute__((alias("gj_locale_tag_lang16_5654")));

uint32_t __gj_batch_id_5654(void)
    __attribute__((alias("gj_batch_id_5654")));
