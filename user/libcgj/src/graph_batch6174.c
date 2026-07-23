/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6174: Flatpak app-id segment count stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_appid_seg_count_6174(const char *app_id);
 *     - Soft count of reverse-DNS segments in a Flatpak app id
 *       (number of '.' separators + 1 when non-empty). Walks at most
 *       255 bytes. NULL or empty → 0. Does not validate char set.
 *   uint32_t __gj_flatpak_appid_seg_count_6174  (alias)
 *   __libcgj_batch6174_marker = "libcgj-batch6174"
 *
 * Exclusive continuum CREATE-ONLY (6171-6180: flatpak app id hash stubs).
 * Unique gj_flatpak_appid_seg_count_6174 surface only; no multi-def.
 * Distinct from gj_flatpak_appid_shape_ok_6175. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6174_marker[] = "libcgj-batch6174";

#define B6174_APPID_MAX  255u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6174_seg_count(const char *szId)
{
	uint32_t u32Dots;
	size_t i;

	if (szId == NULL || szId[0] == '\0') {
		return 0u;
	}
	u32Dots = 0u;
	i = 0u;
	while (szId[i] != '\0' && i < B6174_APPID_MAX) {
		if (szId[i] == '.') {
			u32Dots++;
		}
		i++;
	}
	/* segments = dots + 1 for a non-empty string */
	return u32Dots + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_appid_seg_count_6174 - count reverse-DNS segments.
 *
 * app_id: NUL-terminated Flatpak app id (NULL/empty → 0)
 *
 * Returns soft segment count (dots + 1), walking at most 255 bytes.
 * Pure data; no validation of empty segments. No parent wires.
 */
uint32_t
gj_flatpak_appid_seg_count_6174(const char *app_id)
{
	(void)NULL;
	return b6174_seg_count(app_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_appid_seg_count_6174(const char *app_id)
    __attribute__((alias("gj_flatpak_appid_seg_count_6174")));
