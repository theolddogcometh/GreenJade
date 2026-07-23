/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1701: FNV-1a 64-bit single-byte update.
 *
 * Surface (unique symbols):
 *   uint64_t gj_fnv1a64_update(uint64_t h, uint8_t b);
 *     — Fold one octet into a running FNV-1a 64-bit state:
 *       h ^= b; h *= 1099511628211 (FNV-1a 64-bit prime).
 *   uint64_t __gj_fnv1a64_update  (alias)
 *   __libcgj_batch1701_marker = "libcgj-batch1701"
 *
 * Streaming step companion to one-shot gj_fnv1a64 (batch264). Does NOT
 * redefine gj_fnv1a64 / fnv1a_64 / gj_fnv1a64_n.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1701_marker[] = "libcgj-batch1701";

/* FNV-1a 64-bit prime (public domain constant). */
#define B1701_FNV64_PRIME 1099511628211ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1701_fnv1a64_step(uint64_t u64H, uint8_t u8B)
{
	u64H ^= (uint64_t)u8B;
	u64H *= B1701_FNV64_PRIME;
	return u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fnv1a64_update — fold one byte into a running FNV-1a 64-bit hash.
 *
 * u64H: current hash state (typically offset basis 14695981039346656037
 *       on the first byte, or the prior update result).
 * u8B:  next input octet.
 *
 * Returns the updated 64-bit FNV-1a state.
 */
uint64_t
gj_fnv1a64_update(uint64_t u64H, uint8_t u8B)
{
	return b1701_fnv1a64_step(u64H, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_fnv1a64_update(uint64_t u64H, uint8_t u8B)
    __attribute__((alias("gj_fnv1a64_update")));
