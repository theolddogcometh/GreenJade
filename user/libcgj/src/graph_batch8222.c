/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8222: bounded strnlen stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strnlen_u_8222(const char *s, uint32_t n);
 *     - Return min(strlen(s), n) without reading past n octets.
 *       NULL → 0. n of 0 → 0 without reading.
 *   uint32_t __gj_strnlen_u_8222  (alias)
 *   __libcgj_batch8222_marker = "libcgj-batch8222"
 *
 * Exclusive continuum CREATE-ONLY (8221-8230: string length / bound
 * helpers — strlen_cap, strnlen, strlen_empty, strlen_nonzero,
 * str_bound_ok, str_need_nul, str_fits, str_overflow, str_underflow,
 * batch_id_8230). Unique gj_strnlen_u_8222 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8222_marker[] = "libcgj-batch8222";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8222_strnlen(const char *s, uint32_t u32N)
{
	uint32_t u32I;

	if (s == NULL) {
		return 0u;
	}
	for (u32I = 0u; u32I < u32N; u32I++) {
		if (s[u32I] == '\0') {
			return u32I;
		}
	}
	return u32N;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strnlen_u_8222 - freestanding strnlen-style bounded length.
 *
 * s: candidate C string (may be NULL)
 * n: maximum octets to examine
 *
 * Returns the number of non-NUL octets observed, not exceeding n.
 * Soft freestanding scan; does not call strnlen(3). No parent wires.
 */
uint32_t
gj_strnlen_u_8222(const char *s, uint32_t u32N)
{
	(void)NULL;
	return b8222_strnlen(s, u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strnlen_u_8222(const char *s, uint32_t u32N)
    __attribute__((alias("gj_strnlen_u_8222")));
