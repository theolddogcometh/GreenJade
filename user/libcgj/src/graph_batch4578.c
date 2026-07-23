/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4578: bump major of a packed u32 semver.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ver_bump_major_u(uint32_t v);
 *     - Return a packed semver with major incremented by 1 (saturating
 *       at 255), minor reset to 0, and patch reset to 0. Layout matches
 *       gj_ver_pack_u (batch4571): major[23:16]|minor[15:8]|patch[7:0].
 *   uint32_t __gj_ver_bump_major_u  (alias)
 *   __libcgj_batch4578_marker = "libcgj-batch4578"
 *
 * Exclusive continuum CREATE-ONLY (4571-4580). Unique
 * gj_ver_bump_major_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4578_marker[] = "libcgj-batch4578";

/* Packed semver field layout (matches batch4571). */
#define B4578_MAJ_SHIFT  16
#define B4578_COMP_MASK  ((uint32_t)0xFFu)
#define B4578_COMP_MAX   ((uint32_t)0xFFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4578_major(uint32_t u32V)
{
	return (u32V >> B4578_MAJ_SHIFT) & B4578_COMP_MASK;
}

static uint32_t
b4578_pack(uint32_t u32Maj, uint32_t u32Min, uint32_t u32Pat)
{
	return ((u32Maj & B4578_COMP_MASK) << 16) |
	       ((u32Min & B4578_COMP_MASK) << 8) |
	       (u32Pat & B4578_COMP_MASK);
}

static uint32_t
b4578_bump_major(uint32_t u32V)
{
	uint32_t u32Maj;

	u32Maj = b4578_major(u32V);
	if (u32Maj < B4578_COMP_MAX) {
		u32Maj = u32Maj + 1u;
	}
	/* Minor and patch reset on major bump (semver convention). */
	return b4578_pack(u32Maj, 0u, 0u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ver_bump_major_u - increment major; reset minor and patch to 0.
 *
 * v: packed version word
 *
 * Returns the bumped packed word. Major saturates at 255. Example:
 * gj_ver_bump_major_u(0x00010203u) -> 0x00020000u. No parent wires.
 */
uint32_t
gj_ver_bump_major_u(uint32_t u32V)
{
	(void)NULL;
	return b4578_bump_major(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ver_bump_major_u(uint32_t u32V)
    __attribute__((alias("gj_ver_bump_major_u")));
