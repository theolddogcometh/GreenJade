/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6177: Flatpak app-id dual-hash mix stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_appid_mix32_6177(uint32_t h_a, uint32_t h_b);
 *     - Soft mix of two 32-bit app-id hash halves (e.g. FNV + djb2)
 *       into one 32-bit product key via xor-shift + multiply avalanche.
 *       Pure integer; no string walk.
 *   uint32_t __gj_flatpak_appid_mix32_6177  (alias)
 *   __libcgj_batch6177_marker = "libcgj-batch6177"
 *
 * Exclusive continuum CREATE-ONLY (6171-6180: flatpak app id hash stubs).
 * Unique gj_flatpak_appid_mix32_6177 surface only; no multi-def.
 * Pair with fnv1a32_6171 / djb2_6172. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6177_marker[] = "libcgj-batch6177";

/* Soft avalanche constant (odd). */
#define B6177_MIX_K  0x9e3779b9u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6177_mix32(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32X;

	u32X = u32A ^ (u32B + B6177_MIX_K + (u32A << 6) + (u32A >> 2));
	u32X ^= u32X >> 16;
	u32X *= 0x7feb352du;
	u32X ^= u32X >> 15;
	u32X *= 0x846ca68bu;
	u32X ^= u32X >> 16;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_appid_mix32_6177 - mix two app-id hash halves.
 *
 * h_a: first 32-bit digest (e.g. FNV-1a)
 * h_b: second 32-bit digest (e.g. djb2)
 *
 * Returns a soft avalanche-mixed 32-bit product key. Integer-only.
 * No parent wires.
 */
uint32_t
gj_flatpak_appid_mix32_6177(uint32_t h_a, uint32_t h_b)
{
	(void)NULL;
	return b6177_mix32(h_a, h_b);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_appid_mix32_6177(uint32_t h_a, uint32_t h_b)
    __attribute__((alias("gj_flatpak_appid_mix32_6177")));
