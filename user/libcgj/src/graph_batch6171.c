/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6171: Flatpak app-id FNV-1a 32-bit hash stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_appid_fnv1a32_6171(const char *app_id);
 *     - Soft FNV-1a 32-bit digest of a NUL-terminated Flatpak reverse-DNS
 *       app id (e.g. "org.example.App"). Walks at most 255 bytes. NULL
 *       or empty → 0. Pure data hash; does not contact D-Bus or ostree.
 *   uint32_t __gj_flatpak_appid_fnv1a32_6171  (alias)
 *   __libcgj_batch6171_marker = "libcgj-batch6171"
 *
 * Exclusive continuum CREATE-ONLY (6171-6180: flatpak app id hash stubs).
 * Unique gj_flatpak_appid_fnv1a32_6171 surface only; no multi-def.
 * Distinct from gj_fnv1a64 (batch264) / fnv1a_32 (batch41). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6171_marker[] = "libcgj-batch6171";

/* FNV-1a 32-bit parameters (public domain constants). */
#define B6171_FNV32_OFFSET  2166136261u
#define B6171_FNV32_PRIME   16777619u
/* Soft max app-id walk length (Flatpak id practical bound). */
#define B6171_APPID_MAX     255u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6171_fnv1a32(const char *szId)
{
	uint32_t u32H;
	size_t i;

	if (szId == NULL || szId[0] == '\0') {
		return 0u;
	}
	u32H = B6171_FNV32_OFFSET;
	i = 0u;
	while (szId[i] != '\0' && i < B6171_APPID_MAX) {
		u32H ^= (uint32_t)(unsigned char)szId[i];
		u32H *= B6171_FNV32_PRIME;
		i++;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_appid_fnv1a32_6171 - FNV-1a 32 of a Flatpak app id string.
 *
 * app_id: NUL-terminated reverse-DNS id (NULL/empty → 0)
 *
 * Returns the soft FNV-1a 32-bit digest over at most 255 bytes.
 * Integer/pointer only; no OS or flatpak bus calls. No parent wires.
 */
uint32_t
gj_flatpak_appid_fnv1a32_6171(const char *app_id)
{
	(void)NULL;
	return b6171_fnv1a32(app_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_appid_fnv1a32_6171(const char *app_id)
    __attribute__((alias("gj_flatpak_appid_fnv1a32_6171")));
