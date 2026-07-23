/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6175: Flatpak app-id shape soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_appid_shape_ok_6175(const char *app_id);
 *     - Soft reverse-DNS shape check for a Flatpak app id: length
 *       1..255, only A-Za-z0-9/_/-/. characters, no leading/trailing
 *       '.', no empty segments (".."), at least two segments (one '.').
 *       NULL/bad shape → 0, else 1. Pure data; no D-Bus.
 *   uint32_t __gj_flatpak_appid_shape_ok_6175  (alias)
 *   __libcgj_batch6175_marker = "libcgj-batch6175"
 *
 * Exclusive continuum CREATE-ONLY (6171-6180: flatpak app id hash stubs).
 * Unique gj_flatpak_appid_shape_ok_6175 surface only; no multi-def.
 * Distinct from gj_flatpak_ref_ok_hint (batch1958). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6175_marker[] = "libcgj-batch6175";

#define B6175_APPID_MAX  255u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6175_seg_char_ok(unsigned char uCh)
{
	if ((uCh >= 'A' && uCh <= 'Z') ||
	    (uCh >= 'a' && uCh <= 'z') ||
	    (uCh >= '0' && uCh <= '9')) {
		return 1u;
	}
	if (uCh == '-' || uCh == '_') {
		return 1u;
	}
	return 0u;
}

static uint32_t
b6175_shape_ok(const char *szId)
{
	size_t i;
	size_t cbSeg;
	uint32_t u32Dots;

	if (szId == NULL || szId[0] == '\0' || szId[0] == '.') {
		return 0u;
	}

	cbSeg = 0u;
	u32Dots = 0u;
	i = 0u;
	while (szId[i] != '\0') {
		if (i >= B6175_APPID_MAX) {
			return 0u;
		}
		if (szId[i] == '.') {
			if (cbSeg == 0u) {
				return 0u; /* empty segment */
			}
			u32Dots++;
			cbSeg = 0u;
		} else {
			if (!b6175_seg_char_ok((unsigned char)szId[i])) {
				return 0u;
			}
			cbSeg++;
		}
		i++;
	}
	/* final segment non-empty; need at least one separator */
	if (cbSeg == 0u || u32Dots < 1u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_appid_shape_ok_6175 - soft reverse-DNS app-id shape check.
 *
 * app_id: NUL-terminated Flatpak app id (NULL/bad → 0)
 *
 * Returns 1 when shape looks like a multi-segment reverse-DNS id with
 * restricted characters and length <= 255. No parent wires.
 */
uint32_t
gj_flatpak_appid_shape_ok_6175(const char *app_id)
{
	(void)NULL;
	return b6175_shape_ok(app_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_appid_shape_ok_6175(const char *app_id)
    __attribute__((alias("gj_flatpak_appid_shape_ok_6175")));
