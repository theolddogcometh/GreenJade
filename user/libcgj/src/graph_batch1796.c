/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1796: pack three u16 version fields into u64.
 *
 * Surface (unique symbols):
 *   uint64_t gj_version_tuple_pack(uint16_t major, uint16_t minor,
 *                                  uint16_t patch);
 *     — Pack (major, minor, patch) into a single u64 layout:
 *         bits [47:32] = major, [31:16] = minor, [15:0] = patch.
 *       Upper 16 bits of the u64 are zero. Lossless for all u16 inputs.
 *   uint64_t __gj_version_tuple_pack  (alias)
 *   __libcgj_batch1796_marker = "libcgj-batch1796"
 *
 * Version/semver exclusive wave (1791–1799). Clean-room freestanding
 * pure C (integer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1796_marker[] = "libcgj-batch1796";

/* ---- freestanding helpers ---------------------------------------------- */

/* Pack three u16 into layout major@32 | minor@16 | patch@0. */
static uint64_t
b1796_pack(uint16_t uMaj, uint16_t uMin, uint16_t uPat)
{
	uint64_t u;

	u = ((uint64_t)uMaj << 32) | ((uint64_t)uMin << 16) |
	    (uint64_t)uPat;
	return u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_version_tuple_pack — pack three u16 into one u64.
 */
uint64_t
gj_version_tuple_pack(uint16_t uMaj, uint16_t uMin, uint16_t uPat)
{
	return b1796_pack(uMaj, uMin, uPat);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_version_tuple_pack(uint16_t uMaj, uint16_t uMin, uint16_t uPat)
    __attribute__((alias("gj_version_tuple_pack")));
