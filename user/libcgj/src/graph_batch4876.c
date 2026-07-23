/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4876: extract the alpha channel from 0xAARRGGBB.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rgba_a_u(uint32_t c);
 *     - Return the A channel of packed color c as 0..255
 *       ((c >> 24) & 0xff). For 0x00RRGGBB words this returns 0.
 *   uint32_t __gj_rgba_a_u  (alias)
 *   __libcgj_batch4876_marker = "libcgj-batch4876"
 *
 * Exclusive continuum CREATE-ONLY color pack wave (4871-4880). Distinct
 * from gj_rgba_unpack_u32 (batch1684) and rgb channel extractors
 * (batch4873-4875) — unique gj_rgba_a_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4876_marker[] = "libcgj-batch4876";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4876_a(uint32_t u32C)
{
	return (u32C >> 24) & 0xffu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rgba_a_u - extract alpha channel from a packed RGBA word.
 *
 * c: packed color (0xAARRGGBB; RGB-only words yield 0)
 *
 * Returns A in 0..255. Self-contained; no parent wires.
 */
uint32_t
gj_rgba_a_u(uint32_t u32C)
{
	(void)NULL;
	return b4876_a(u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rgba_a_u(uint32_t u32C)
    __attribute__((alias("gj_rgba_a_u")));
