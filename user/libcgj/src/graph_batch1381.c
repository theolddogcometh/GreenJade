/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1381: pack two u32 halves into one u64.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_pack_u32x2(uint32_t lo, uint32_t hi);
 *     — Form ((uint64_t)hi << 32) | lo. lo occupies bits [31:0],
 *       hi occupies bits [63:32].
 *   uint64_t __gj_u64_pack_u32x2  (alias)
 *   __libcgj_batch1381_marker = "libcgj-batch1381"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1381_marker[] = "libcgj-batch1381";

/* ---- freestanding helpers ---------------------------------------------- */

/* Combine lo (low 32) and hi (high 32) into a 64-bit word. */
static uint64_t
b1381_pack(uint32_t u32Lo, uint32_t u32Hi)
{
	return ((uint64_t)u32Hi << 32) | (uint64_t)u32Lo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_pack_u32x2 — pack two uint32 values into one uint64.
 *
 * u32Lo: low half  → bits [31:0]
 * u32Hi: high half → bits [63:32]
 */
uint64_t
gj_u64_pack_u32x2(uint32_t u32Lo, uint32_t u32Hi)
{
	return b1381_pack(u32Lo, u32Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_pack_u32x2(uint32_t u32Lo, uint32_t u32Hi)
    __attribute__((alias("gj_u64_pack_u32x2")));
