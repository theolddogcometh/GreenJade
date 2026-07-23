/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4389: FNV-1a 32-bit single-byte update (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fnv1a32_u8_u(uint8_t byte, uint32_t hash);
 *     - Fold one octet into a running FNV-1a 32-bit state:
 *       hash ^= byte; hash *= 16777619 (FNV-1a 32-bit prime).
 *   uint32_t __gj_fnv1a32_u8_u  (alias)
 *   __libcgj_batch4389_marker = "libcgj-batch4389"
 *
 * Exclusive continuum CREATE-ONLY (4381-4390). Distinct from
 * gj_fnv1a32_u (batch2801), gj_fnv1a32 (batch421), gj_fnv1a64_update
 * (batch1701) — single-byte 32-bit update surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4389_marker[] = "libcgj-batch4389";

/* FNV-1a 32-bit prime (public domain constant). */
#define B4389_FNV32_PRIME 16777619u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4389_fnv1a32_step(uint8_t u8B, uint32_t u32H)
{
	u32H ^= (uint32_t)u8B;
	u32H *= B4389_FNV32_PRIME;
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fnv1a32_u8_u - fold one byte into a running FNV-1a 32-bit hash.
 *
 * byte: next input octet
 * hash: current hash state (typically offset basis 2166136261 on the
 *       first byte, or the prior update result)
 *
 * Returns the updated 32-bit FNV-1a state.
 * Self-contained; no parent wires.
 */
uint32_t
gj_fnv1a32_u8_u(uint8_t u8B, uint32_t u32H)
{
	(void)NULL;
	return b4389_fnv1a32_step(u8B, u32H);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fnv1a32_u8_u(uint8_t u8B, uint32_t u32H)
    __attribute__((alias("gj_fnv1a32_u8_u")));
