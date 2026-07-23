/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8223: string empty-or-null predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strlen_empty_u_8223(const char *s);
 *     - Return 1 if s is NULL or s points at an empty string, else 0.
 *   uint32_t __gj_strlen_empty_u_8223  (alias)
 *   __libcgj_batch8223_marker = "libcgj-batch8223"
 *
 * Exclusive continuum CREATE-ONLY (8221-8230: string length / bound
 * helpers — strlen_cap, strnlen, strlen_empty, strlen_nonzero,
 * str_bound_ok, str_need_nul, str_fits, str_overflow, str_underflow,
 * batch_id_8230). Unique gj_strlen_empty_u_8223 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8223_marker[] = "libcgj-batch8223";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8223_strlen_empty(const char *s)
{
	if (s == NULL) {
		return 1u;
	}
	return (s[0] == '\0') ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strlen_empty_u_8223 - 1 if string is empty or pointer is NULL.
 *
 * s: candidate C string (may be NULL)
 *
 * Returns 1 when s is NULL or *s is NUL, else 0. Soft catalog test;
 * does not call strlen(3). No parent wires.
 */
uint32_t
gj_strlen_empty_u_8223(const char *s)
{
	(void)NULL;
	return b8223_strlen_empty(s);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strlen_empty_u_8223(const char *s)
    __attribute__((alias("gj_strlen_empty_u_8223")));
