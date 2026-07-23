/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4576: semver compatibility of packed u32 words.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ver_compatible_u(uint32_t have, uint32_t need);
 *     - Return 1 when have is API-compatible with need under exclusive
 *       rules: majors equal (bits [23:16]) AND have >= need (packed
 *       numeric order, which matches semver for the 8.8.8 layout).
 *       Otherwise return 0. Models "same major, at least need".
 *   uint32_t __gj_ver_compatible_u  (alias)
 *   __libcgj_batch4576_marker = "libcgj-batch4576"
 *
 * Exclusive continuum CREATE-ONLY (4571-4580). Unique
 * gj_ver_compatible_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4576_marker[] = "libcgj-batch4576";

/* Packed semver major field (matches batch4571 layout). */
#define B4576_MAJ_SHIFT  16
#define B4576_COMP_MASK  ((uint32_t)0xFFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4576_major(uint32_t u32V)
{
	return (u32V >> B4576_MAJ_SHIFT) & B4576_COMP_MASK;
}

static uint32_t
b4576_compatible(uint32_t u32Have, uint32_t u32Need)
{
	if (b4576_major(u32Have) != b4576_major(u32Need)) {
		return 0u;
	}
	if (u32Have >= u32Need) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ver_compatible_u - test packed-semver API compatibility.
 *
 * have: available packed version
 * need: required packed version
 *
 * Returns 1 when majors match and have >= need; else 0. Example:
 * have=1.2.0 need=1.1.0 -> 1; have=2.0.0 need=1.9.0 -> 0 (major).
 * No parent wires.
 */
uint32_t
gj_ver_compatible_u(uint32_t u32Have, uint32_t u32Need)
{
	(void)NULL;
	return b4576_compatible(u32Have, u32Need);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ver_compatible_u(uint32_t u32Have, uint32_t u32Need)
    __attribute__((alias("gj_ver_compatible_u")));
