/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch640: freestanding in-place 32-bit byte-swap
 * of a bounded uint32_t buffer.
 *
 * Surface (unique symbols):
 *   void gj_u32_bswap_buf(uint32_t *a, size_t n);
 *     — For each of a[0..n), reverse the four octets in place
 *       (abcd → dcba). NULL a → no-op; n == 0 → no-op.
 *   void __gj_u32_bswap_buf  (alias)
 *   __libcgj_batch640_marker = "libcgj-batch640"
 *
 * Distinct from gj_bswap32 / __gj_bswap32 (graph_batch416) — scalar
 * single-word surface only; no multi-def. Self-contained bswap (no
 * cross-batch call, no compiler builtins).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch640_marker[] = "libcgj-batch640";

/* ---- freestanding helpers ---------------------------------------------- */

/* Portable 32-bit octet reverse: abcd → dcba. */
static uint32_t
b640_bswap32(uint32_t x)
{
	return ((x & 0x000000ffu) << 24) |
	       ((x & 0x0000ff00u) << 8) |
	       ((x & 0x00ff0000u) >> 8) |
	       ((x & 0xff000000u) >> 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bswap_buf — in-place byte-swap each of n uint32_t elements.
 *
 * a: base of n uint32_t words (NULL → no-op)
 * n: element count (0 → no-op)
 *
 * Mutates a[i] for i in [0, n). Does not call gj_bswap32 or libc.
 */
void
gj_u32_bswap_buf(uint32_t *a, size_t n)
{
	size_t i;

	if (a == NULL || n == 0u) {
		return;
	}

	for (i = 0u; i < n; i++) {
		a[i] = b640_bswap32(a[i]);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_bswap_buf(uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_bswap_buf")));
