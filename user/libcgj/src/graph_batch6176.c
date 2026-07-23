/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6176: Flatpak app-id length soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_appid_len_ok_6176(const char *app_id);
 *     - Soft length gate for a Flatpak app id: non-NULL, length in
 *       [1, 255] inclusive. Does not validate character set. Returns 1
 *       if length is in range, else 0.
 *   uint32_t gj_flatpak_appid_len_6176(const char *app_id);
 *     - Soft byte length of app_id (0 if NULL), capped at 255 report
 *       when longer (returns 256 as "too long" sentinel).
 *   uint32_t __gj_flatpak_appid_len_ok_6176  (alias)
 *   uint32_t __gj_flatpak_appid_len_6176  (alias)
 *   __libcgj_batch6176_marker = "libcgj-batch6176"
 *
 * Exclusive continuum CREATE-ONLY (6171-6180: flatpak app id hash stubs).
 * Unique *_6176 surfaces only; no multi-def. Distinct from shape_ok_6175.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6176_marker[] = "libcgj-batch6176";

#define B6176_APPID_MAX  255u
/* Sentinel: walked past max without NUL. */
#define B6176_TOO_LONG   256u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6176_len(const char *szId)
{
	size_t i;

	if (szId == NULL) {
		return 0u;
	}
	i = 0u;
	while (szId[i] != '\0') {
		if (i >= B6176_APPID_MAX) {
			return B6176_TOO_LONG;
		}
		i++;
	}
	return (uint32_t)i;
}

static uint32_t
b6176_len_ok(const char *szId)
{
	uint32_t u32N;

	u32N = b6176_len(szId);
	if (u32N >= 1u && u32N <= B6176_APPID_MAX) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_appid_len_ok_6176 - soft app-id length gate.
 *
 * app_id: NUL-terminated Flatpak app id
 *
 * Returns 1 if length in [1, 255], else 0. No parent wires.
 */
uint32_t
gj_flatpak_appid_len_ok_6176(const char *app_id)
{
	(void)NULL;
	return b6176_len_ok(app_id);
}

/*
 * gj_flatpak_appid_len_6176 - soft app-id byte length.
 *
 * app_id: NUL-terminated Flatpak app id (NULL → 0)
 *
 * Returns length, or 256 if longer than 255 without a NUL.
 */
uint32_t
gj_flatpak_appid_len_6176(const char *app_id)
{
	return b6176_len(app_id);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_flatpak_appid_len_ok_6176(const char *app_id)
    __attribute__((alias("gj_flatpak_appid_len_ok_6176")));

uint32_t __gj_flatpak_appid_len_6176(const char *app_id)
    __attribute__((alias("gj_flatpak_appid_len_6176")));
