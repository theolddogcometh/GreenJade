/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4579: bump minor of a packed u32 semver.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ver_bump_minor_u(uint32_t v);
 *     - Return a packed semver with minor incremented by 1 (saturating
 *       at 255), major preserved, and patch reset to 0. Layout matches
 *       gj_ver_pack_u (batch4571): major[23:16]|minor[15:8]|patch[7:0].
 *   uint32_t __gj_ver_bump_minor_u  (alias)
 *   __libcgj_batch4579_marker = "libcgj-batch4579"
 *
 * Exclusive continuum CREATE-ONLY (4571-4580). Unique
 * gj_ver_bump_minor_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4579_marker[] = "libcgj-batch4579";

/* Packed semver field layout (matches batch4571). */
#define B4579_MAJ_SHIFT  16
#define B4579_MIN_SHIFT  8
#define B4579_COMP_MASK  ((uint32_t)0xFFu)
#define B4579_COMP_MAX   ((uint32_t)0xFFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4579_major(uint32_t u32V)
{
	return (u32V >> B4579_MAJ_SHIFT) & B4579_COMP_MASK;
}

static uint32_t
b4579_minor(uint32_t u32V)
{
	return (u32V >> B4579_MIN_SHIFT) & B4579_COMP_MASK;
}

static uint32_t
b4579_pack(uint32_t u32Maj, uint32_t u32Min, uint32_t u32Pat)
{
	return ((u32Maj & B4579_COMP_MASK) << 16) |
	       ((u32Min & B4579_COMP_MASK) << 8) |
	       (u32Pat & B4579_COMP_MASK);
}

static uint32_t
b4579_bump_minor(uint32_t u32V)
{
	uint32_t u32Maj;
	uint32_t u32Min;

	u32Maj = b4579_major(u32V);
	u32Min = b4579_minor(u32V);
	if (u32Min < B4579_COMP_MAX) {
		u32Min = u32Min + 1u;
	}
	/* Patch resets on minor bump (semver convention). */
	return b4579_pack(u32Maj, u32Min, 0u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ver_bump_minor_u - increment minor; preserve major; reset patch.
 *
 * v: packed version word
 *
 * Returns the bumped packed word. Minor saturates at 255. Example:
 * gj_ver_bump_minor_u(0x00010203u) -> 0x00010300u. No parent wires.
 */
uint32_t
gj_ver_bump_minor_u(uint32_t u32V)
{
	(void)NULL;
	return b4579_bump_minor(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ver_bump_minor_u(uint32_t u32V)
    __attribute__((alias("gj_ver_bump_minor_u")));
