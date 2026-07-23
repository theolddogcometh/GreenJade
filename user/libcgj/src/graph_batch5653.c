/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5653: first-boot locale pack (lang/region).
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_pack_5653(uint32_t lang, uint32_t region,
 *                                uint32_t script);
 *     - Pack locale selection for wizard state:
 *         bit0..9   = lang id (0..1023)
 *         bit10..19 = region id (0..1023)
 *         bit20..23 = script nibble (0..15)
 *         bit24..27 = mix nibble
 *   uint32_t gj_locale_lang_5653(uint32_t packed);
 *   uint32_t gj_locale_region_5653(uint32_t packed);
 *   uint32_t gj_locale_script_5653(uint32_t packed);
 *   uint32_t gj_batch_id_5653(void);
 *   __ aliases
 *   __libcgj_batch5653_marker = "libcgj-batch5653"
 *
 * Exclusive continuum CREATE-ONLY (5651-5660: post-install first-boot
 * wizard stubs, locale, keyboard, network setup flags). Unique *_5653
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5653_marker[] = "libcgj-batch5653";

#define B5653_BATCH_ID 5653u
#define B5653_LANG_MAX 1023u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5653_clamp10(uint32_t u32V)
{
	if (u32V > B5653_LANG_MAX) {
		return B5653_LANG_MAX;
	}
	return u32V;
}

static uint32_t
b5653_mix4(uint32_t u32X)
{
	u32X ^= u32X >> 16;
	u32X *= 0x45d9f3bu;
	u32X ^= u32X >> 16;
	return u32X & 0xfu;
}

static uint32_t
b5653_pack(uint32_t u32Lang, uint32_t u32Region, uint32_t u32Script)
{
	uint32_t u32P;

	u32Lang = b5653_clamp10(u32Lang);
	u32Region = b5653_clamp10(u32Region);
	u32Script = u32Script & 0xfu;

	u32P = u32Lang | (u32Region << 10) | (u32Script << 20);
	return u32P | (b5653_mix4(u32P ^ B5653_BATCH_ID) << 24);
}

/* ---- public surface ---------------------------------------------------- */

uint32_t
gj_locale_pack_5653(uint32_t lang, uint32_t region, uint32_t script)
{
	(void)NULL;
	return b5653_pack(lang, region, script);
}

uint32_t
gj_locale_lang_5653(uint32_t packed)
{
	return packed & 0x3ffu;
}

uint32_t
gj_locale_region_5653(uint32_t packed)
{
	return (packed >> 10) & 0x3ffu;
}

uint32_t
gj_locale_script_5653(uint32_t packed)
{
	return (packed >> 20) & 0xfu;
}

uint32_t
gj_batch_id_5653(void)
{
	return B5653_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_locale_pack_5653(uint32_t lang, uint32_t region,
    uint32_t script)
    __attribute__((alias("gj_locale_pack_5653")));

uint32_t __gj_locale_lang_5653(uint32_t packed)
    __attribute__((alias("gj_locale_lang_5653")));

uint32_t __gj_locale_region_5653(uint32_t packed)
    __attribute__((alias("gj_locale_region_5653")));

uint32_t __gj_locale_script_5653(uint32_t packed)
    __attribute__((alias("gj_locale_script_5653")));

uint32_t __gj_batch_id_5653(void)
    __attribute__((alias("gj_batch_id_5653")));
