/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4574: extract patch from a packed u32 semver.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ver_patch_u(uint32_t v);
 *     - Return the patch field of packed semver v (bits [7:0], range
 *       0..255). Layout matches gj_ver_pack_u (batch4571): major[23:16]
 *       | minor[15:8] | patch[7:0]. Self-contained extract; no parent
 *       wires.
 *   uint32_t __gj_ver_patch_u  (alias)
 *   __libcgj_batch4574_marker = "libcgj-batch4574"
 *
 * Exclusive continuum CREATE-ONLY (4571-4580). Unique gj_ver_patch_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4574_marker[] = "libcgj-batch4574";

/* Packed semver patch field (matches batch4571 layout). */
#define B4574_PAT_SHIFT  0
#define B4574_COMP_MASK  ((uint32_t)0xFFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4574_patch(uint32_t u32V)
{
	return (u32V >> B4574_PAT_SHIFT) & B4574_COMP_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ver_patch_u - extract the patch component of a packed u32 semver.
 *
 * v: packed version word (layout major[23:16]|minor[15:8]|patch[7:0])
 *
 * Returns patch in 0..255. Example: gj_ver_patch_u(0x00010203u) -> 3.
 * No parent wires.
 */
uint32_t
gj_ver_patch_u(uint32_t u32V)
{
	(void)NULL;
	return b4574_patch(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ver_patch_u(uint32_t u32V)
    __attribute__((alias("gj_ver_patch_u")));
