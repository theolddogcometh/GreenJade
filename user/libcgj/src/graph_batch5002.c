/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5002: broadcast a byte across a uint64_t word.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_broadcast_byte_u(uint32_t b);
 *     - Fill all 8 bytes of a uint64_t with the low 8 bits of b.
 *   uint64_t __gj_u64_broadcast_byte_u  (alias)
 *   __libcgj_batch5002_marker = "libcgj-batch5002"
 *
 * Exclusive continuum CREATE-ONLY (5001-5010). Distinct from
 * gj_u32_broadcast_byte_u (batch5001) — unique gj_u64_broadcast_byte_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5002_marker[] = "libcgj-batch5002";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5002_broadcast_byte(uint32_t u32B)
{
	uint64_t u64V;

	u64V = (uint64_t)(u32B & 0xFFu);
	u64V |= u64V << 8;
	u64V |= u64V << 16;
	u64V |= u64V << 32;
	return u64V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_broadcast_byte_u - replicate low 8 bits of b into all 8 bytes.
 *
 * b: source byte (only bits 7:0 used)
 *
 * Returns a 64-bit word whose every byte equals (b & 0xFF). Example:
 * gj_u64_broadcast_byte_u(0xABu) -> 0xABABABABABABABABull.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_broadcast_byte_u(uint32_t u32B)
{
	(void)NULL;
	return b5002_broadcast_byte(u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_broadcast_byte_u(uint32_t u32B)
    __attribute__((alias("gj_u64_broadcast_byte_u")));
