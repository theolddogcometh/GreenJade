/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6173: Flatpak app-id character soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_appid_char_ok_6173(uint32_t ch);
 *     - Soft validity for one code unit of a Flatpak reverse-DNS app id.
 *       Accepts A-Za-z0-9, '-', '_', and '.' (segment separator). Low
 *       8 bits of ch are the character; upper bits ignored. Returns 1
 *       if ok, else 0.
 *   uint32_t __gj_flatpak_appid_char_ok_6173  (alias)
 *   __libcgj_batch6173_marker = "libcgj-batch6173"
 *
 * Exclusive continuum CREATE-ONLY (6171-6180: flatpak app id hash stubs).
 * Unique gj_flatpak_appid_char_ok_6173 surface only; no multi-def.
 * Distinct from gj_flatpak_ref_ok_hint (batch1958) segment char check.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6173_marker[] = "libcgj-batch6173";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6173_char_ok(uint32_t u32Ch)
{
	unsigned char uCh;

	uCh = (unsigned char)(u32Ch & 0xffu);
	if ((uCh >= 'A' && uCh <= 'Z') ||
	    (uCh >= 'a' && uCh <= 'z') ||
	    (uCh >= '0' && uCh <= '9')) {
		return 1u;
	}
	if (uCh == '-' || uCh == '_' || uCh == '.') {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_appid_char_ok_6173 - soft app-id character validity.
 *
 * ch: candidate code unit (low 8 bits used)
 *
 * Returns 1 if the character is allowed in a reverse-DNS Flatpak app
 * id segment or as '.', else 0. Soft pure-data. No parent wires.
 */
uint32_t
gj_flatpak_appid_char_ok_6173(uint32_t ch)
{
	(void)NULL;
	return b6173_char_ok(ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_appid_char_ok_6173(uint32_t ch)
    __attribute__((alias("gj_flatpak_appid_char_ok_6173")));
