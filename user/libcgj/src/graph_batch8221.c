/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8221: string length capped count stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strlen_cap_u_8221(const char *s, uint32_t cap);
 *     - Count octets until NUL or cap is reached; return length in
 *       [0, cap]. NULL → 0. Cap of 0 → 0 without reading.
 *   uint32_t __gj_strlen_cap_u_8221  (alias)
 *   __libcgj_batch8221_marker = "libcgj-batch8221"
 *
 * Exclusive continuum CREATE-ONLY (8221-8230: string length / bound
 * helpers — strlen_cap, strnlen, strlen_empty, strlen_nonzero,
 * str_bound_ok, str_need_nul, str_fits, str_overflow, str_underflow,
 * batch_id_8230). Unique gj_strlen_cap_u_8221 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8221_marker[] = "libcgj-batch8221";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8221_strlen_cap(const char *s, uint32_t u32Cap)
{
	uint32_t u32I;

	if (s == NULL) {
		return 0u;
	}
	for (u32I = 0u; u32I < u32Cap; u32I++) {
		if (s[u32I] == '\0') {
			return u32I;
		}
	}
	return u32Cap;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strlen_cap_u_8221 - count string length up to a hard cap.
 *
 * s:   candidate C string (may be NULL)
 * cap: maximum octets to examine / report
 *
 * Returns the number of non-NUL octets observed, not exceeding cap.
 * Soft freestanding scan; does not call strlen(3). No parent wires.
 */
uint32_t
gj_strlen_cap_u_8221(const char *s, uint32_t u32Cap)
{
	(void)NULL;
	return b8221_strlen_cap(s, u32Cap);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strlen_cap_u_8221(const char *s, uint32_t u32Cap)
    __attribute__((alias("gj_strlen_cap_u_8221")));
