/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6178: Flatpak app-id prefix (first segment)
 * FNV-1a 32 hash stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_appid_prefix_hash_6178(const char *app_id);
 *     - Soft FNV-1a 32 digest of the first reverse-DNS segment only
 *       (bytes before the first '.', or whole string if no '.'). Walks
 *       at most 255 bytes. NULL or empty → 0. Useful for vendor-prefix
 *       bucketing (e.g. "org", "com").
 *   uint32_t __gj_flatpak_appid_prefix_hash_6178  (alias)
 *   __libcgj_batch6178_marker = "libcgj-batch6178"
 *
 * Exclusive continuum CREATE-ONLY (6171-6180: flatpak app id hash stubs).
 * Unique gj_flatpak_appid_prefix_hash_6178 surface only; no multi-def.
 * Distinct from gj_flatpak_appid_fnv1a32_6171 (full string). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6178_marker[] = "libcgj-batch6178";

#define B6178_FNV32_OFFSET  2166136261u
#define B6178_FNV32_PRIME   16777619u
#define B6178_APPID_MAX     255u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6178_prefix_fnv(const char *szId)
{
	uint32_t u32H;
	size_t i;

	if (szId == NULL || szId[0] == '\0') {
		return 0u;
	}
	u32H = B6178_FNV32_OFFSET;
	i = 0u;
	while (szId[i] != '\0' && szId[i] != '.' && i < B6178_APPID_MAX) {
		u32H ^= (uint32_t)(unsigned char)szId[i];
		u32H *= B6178_FNV32_PRIME;
		i++;
	}
	/* empty prefix (leading '.') still returns offset basis if we
	 * never ate a char — treat as zero for soft bucketing */
	if (i == 0u) {
		return 0u;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_appid_prefix_hash_6178 - FNV-1a 32 of first DNS segment.
 *
 * app_id: NUL-terminated Flatpak app id (NULL/empty → 0)
 *
 * Returns soft FNV-1a 32 of the vendor/prefix segment. No parent wires.
 */
uint32_t
gj_flatpak_appid_prefix_hash_6178(const char *app_id)
{
	(void)NULL;
	return b6178_prefix_fnv(app_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_appid_prefix_hash_6178(const char *app_id)
    __attribute__((alias("gj_flatpak_appid_prefix_hash_6178")));
