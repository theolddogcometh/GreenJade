/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4571: pack major/minor/patch into a u32 semver.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ver_pack_u(uint32_t maj, uint32_t min, uint32_t pat);
 *     - Pack (maj, min, pat) into a uint32_t using the exclusive layout
 *       major[23:16] | minor[15:8] | patch[7:0] (each field masked to 8
 *       bits; bits [31:24] are zero). Lexicographic order of the packed
 *       word matches semver order for fields in range.
 *   uint32_t __gj_ver_pack_u  (alias)
 *   __libcgj_batch4571_marker = "libcgj-batch4571"
 *
 * Exclusive continuum CREATE-ONLY (4571-4580: ver_pack_u, ver_major_u,
 * ver_minor_u, ver_patch_u, ver_cmp_u, ver_compatible_u, ver_is_zero_u,
 * ver_bump_major_u, ver_bump_minor_u, batch_id_4580). Unique
 * gj_ver_pack_u surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4571_marker[] = "libcgj-batch4571";

/* Packed semver field layout (8.8.8, high byte reserved). */
#define B4571_MAJ_SHIFT  16
#define B4571_MIN_SHIFT  8
#define B4571_PAT_SHIFT  0
#define B4571_COMP_MASK  ((uint32_t)0xFFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4571_pack(uint32_t u32Maj, uint32_t u32Min, uint32_t u32Pat)
{
	return ((u32Maj & B4571_COMP_MASK) << B4571_MAJ_SHIFT) |
	       ((u32Min & B4571_COMP_MASK) << B4571_MIN_SHIFT) |
	       ((u32Pat & B4571_COMP_MASK) << B4571_PAT_SHIFT);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ver_pack_u - pack major/minor/patch into a packed u32 semver word.
 *
 * maj: major component (low 8 bits used)
 * min: minor component (low 8 bits used)
 * pat: patch component (low 8 bits used)
 *
 * Returns the packed word. Example: gj_ver_pack_u(1, 2, 3) -> 0x00010203.
 * Self-contained; no parent wires.
 */
uint32_t
gj_ver_pack_u(uint32_t u32Maj, uint32_t u32Min, uint32_t u32Pat)
{
	(void)NULL;
	return b4571_pack(u32Maj, u32Min, u32Pat);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ver_pack_u(uint32_t u32Maj, uint32_t u32Min, uint32_t u32Pat)
    __attribute__((alias("gj_ver_pack_u")));
