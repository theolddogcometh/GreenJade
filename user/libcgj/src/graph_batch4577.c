/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4577: test whether a packed u32 semver is zero.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ver_is_zero_u(uint32_t v);
 *     - Return 1 when the packed semver word v is exactly 0 (major,
 *       minor, and patch all zero / high reserved bits clear). Return 0
 *       otherwise. Useful as a "version unset" probe.
 *   uint32_t __gj_ver_is_zero_u  (alias)
 *   __libcgj_batch4577_marker = "libcgj-batch4577"
 *
 * Exclusive continuum CREATE-ONLY (4571-4580). Unique gj_ver_is_zero_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4577_marker[] = "libcgj-batch4577";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4577_is_zero(uint32_t u32V)
{
	if (u32V == 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ver_is_zero_u - test whether a packed u32 semver is the zero word.
 *
 * v: packed version word
 *
 * Returns 1 when v == 0, else 0. Example: gj_ver_is_zero_u(0) -> 1;
 * gj_ver_is_zero_u(0x00010000u) -> 0. No parent wires.
 */
uint32_t
gj_ver_is_zero_u(uint32_t u32V)
{
	(void)NULL;
	return b4577_is_zero(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ver_is_zero_u(uint32_t u32V)
    __attribute__((alias("gj_ver_is_zero_u")));
