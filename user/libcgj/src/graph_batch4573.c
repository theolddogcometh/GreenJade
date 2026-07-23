/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4573: extract minor from a packed u32 semver.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ver_minor_u(uint32_t v);
 *     - Return the minor field of packed semver v (bits [15:8], range
 *       0..255). Layout matches gj_ver_pack_u (batch4571): major[23:16]
 *       | minor[15:8] | patch[7:0]. Self-contained extract; no parent
 *       wires.
 *   uint32_t __gj_ver_minor_u  (alias)
 *   __libcgj_batch4573_marker = "libcgj-batch4573"
 *
 * Exclusive continuum CREATE-ONLY (4571-4580). Unique gj_ver_minor_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4573_marker[] = "libcgj-batch4573";

/* Packed semver minor field (matches batch4571 layout). */
#define B4573_MIN_SHIFT  8
#define B4573_COMP_MASK  ((uint32_t)0xFFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4573_minor(uint32_t u32V)
{
	return (u32V >> B4573_MIN_SHIFT) & B4573_COMP_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ver_minor_u - extract the minor component of a packed u32 semver.
 *
 * v: packed version word (layout major[23:16]|minor[15:8]|patch[7:0])
 *
 * Returns minor in 0..255. Example: gj_ver_minor_u(0x00010203u) -> 2.
 * No parent wires.
 */
uint32_t
gj_ver_minor_u(uint32_t u32V)
{
	(void)NULL;
	return b4573_minor(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ver_minor_u(uint32_t u32V)
    __attribute__((alias("gj_ver_minor_u")));
