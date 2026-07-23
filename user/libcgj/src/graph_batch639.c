/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch639: in-place endian swap of a uint16_t buffer.
 *
 * Surface (unique symbols):
 *   void gj_u16_bswap_buf(uint16_t *a, size_t n);
 *     — For each of a[0..n), reverse the two octets in place
 *       (portable pure-C 16-bit bswap). a == NULL → no-op.
 *       n == 0 → no-op.
 *   void __gj_u16_bswap_buf  (alias)
 *   __libcgj_batch639_marker = "libcgj-batch639"
 *
 * Distinct from scalar gj_bswap32 / gj_bswap64 (batch416/417) and from
 * gj_ntohs16 / gj_htons16 (batch488/486) — buffer form under unique names.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No compiler builtins.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch639_marker[] = "libcgj-batch639";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_bswap_buf — in-place 16-bit endian swap over a[0..n).
 *
 * a: array of at least n uint16_t elements when n > 0 (NULL → no-op)
 * n: element count (0 → no-op)
 *
 * Each element is transformed as (lo,hi) ↔ (hi,lo). Self-contained;
 * does not call cross-batch scalar bswap helpers.
 */
void
gj_u16_bswap_buf(uint16_t *a, size_t n)
{
	size_t i;

	if (a == NULL) {
		return;
	}
	if (n == 0u) {
		return;
	}

	for (i = 0u; i < n; i++) {
		uint16_t x = a[i];

		a[i] = (uint16_t)(((x & 0x00ffu) << 8) | ((x & 0xff00u) >> 8));
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u16_bswap_buf(uint16_t *a, size_t n)
    __attribute__((alias("gj_u16_bswap_buf")));
