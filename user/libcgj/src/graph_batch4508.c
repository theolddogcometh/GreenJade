/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4508: extract bit i of a uint32_t as 0/1.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_unpack_bool_u(uint32_t x, uint32_t i);
 *     - Return ((x >> (i & 31)) & 1). i is taken modulo 32 so out-of-range
 *       indices wrap safely without UB on shift.
 *   uint32_t __gj_u32_unpack_bool_u  (alias)
 *   __libcgj_batch4508_marker = "libcgj-batch4508"
 *
 * Exclusive continuum CREATE-ONLY (4501-4510). Distinct from
 * gj_unpack_bools8 (batch933) and gj_u32_pack_bool4_u (batch4507) —
 * unique gj_u32_unpack_bool_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4508_marker[] = "libcgj-batch4508";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4508_unpack_bool(uint32_t u32X, uint32_t uI)
{
	return (u32X >> (uI & 31u)) & 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_unpack_bool_u - extract bit i of x as a 0/1 bool.
 *
 * x: source word
 * i: bit index (modulo 32)
 *
 * Returns 0 or 1. Example:
 * gj_u32_unpack_bool_u(0xA5u, 0) -> 1; gj_u32_unpack_bool_u(0xA5u, 1) -> 0.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_unpack_bool_u(uint32_t u32X, uint32_t uI)
{
	(void)NULL;
	return b4508_unpack_bool(u32X, uI);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_unpack_bool_u(uint32_t u32X, uint32_t uI)
    __attribute__((alias("gj_u32_unpack_bool_u")));
