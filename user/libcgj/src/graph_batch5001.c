/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5001: broadcast a byte across a uint32_t word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_broadcast_byte_u(uint32_t b);
 *     - Fill all 4 bytes of a uint32_t with the low 8 bits of b.
 *       Equivalent to 0x01010101u * (b & 0xFFu).
 *   uint32_t __gj_u32_broadcast_byte_u  (alias)
 *   __libcgj_batch5001_marker = "libcgj-batch5001"
 *
 * Exclusive continuum CREATE-ONLY (5001-5010: u32_broadcast_byte_u,
 * u64_broadcast_byte_u, u32_has_zero_byte_u, u64_has_zero_byte_u,
 * u32_has_byte_u, u32_count_bytes_eq_u, u32_first_byte_eq_u,
 * u32_last_byte_eq_u, u32_bytes_sum_u, batch_id_5010). Post-5000
 * identity/bit unique wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5001_marker[] = "libcgj-batch5001";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5001_broadcast_byte(uint32_t u32B)
{
	uint32_t u32V;

	u32V = u32B & 0xFFu;
	return (u32V) | (u32V << 8) | (u32V << 16) | (u32V << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_broadcast_byte_u - replicate low 8 bits of b into all 4 bytes.
 *
 * b: source byte (only bits 7:0 used)
 *
 * Returns a word whose every byte equals (b & 0xFF). Example:
 * gj_u32_broadcast_byte_u(0xABu) -> 0xABABABABu.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_broadcast_byte_u(uint32_t u32B)
{
	(void)NULL;
	return b5001_broadcast_byte(u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_broadcast_byte_u(uint32_t u32B)
    __attribute__((alias("gj_u32_broadcast_byte_u")));
