/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch660: length-bounded ASCII horizontal/vertical
 * whitespace predicate (space / tab / CR / LF only).
 *
 * Surface (unique symbols):
 *   int gj_ascii_is_space_n(const char *s, size_t n);
 *     — Return 1 (OK) if every octet in s[0..n) is space (0x20), tab
 *       (0x09), CR (0x0D), or LF (0x0A). Else return 0 (FAIL). n == 0
 *       is vacuously OK (returns 1) even if s is NULL. If n > 0 and s
 *       is NULL, returns 0 (FAIL). Does not stop at NUL; walks exactly
 *       n bytes. Does not treat VT/FF/NBSP or locale isspace classes
 *       as space.
 *   int __gj_ascii_is_space_n  (alias)
 *   __libcgj_batch660_marker = "libcgj-batch660"
 *
 * Distinct from ctype isspace / isspace_l (locale-aware, single code
 * unit) and from gj_str_ltrim / gj_str_rtrim helpers that skip the same
 * four octets but return pointers or lengths. Unique gj_ names only;
 * no multi-def with the existing libc graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc helpers (no isspace / strlen). No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch660_marker[] = "libcgj-batch660";

/* ---- freestanding helpers (not exported) ------------------------------- */

/* True if ch is space, tab, CR, or LF (ASCII only; no VT/FF/NBSP). */
static int
b660_is_space(unsigned char u8Ch)
{
	return u8Ch == (unsigned char)' ' ||
	    u8Ch == (unsigned char)'\t' ||
	    u8Ch == (unsigned char)'\r' ||
	    u8Ch == (unsigned char)'\n';
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_is_space_n — true (OK) if s[0..n) is only space/tab/CR/LF.
 *
 * s: region of at least n bytes when n > 0 (NULL with n > 0 → 0 / FAIL)
 * n: number of octets to scan (0 → 1 / OK, empty window is vacuously
 *    all-space)
 *
 * Does not call libc. Early-exits on the first non-space octet.
 * Returns 1 on success (OK), 0 otherwise (FAIL).
 */
int
gj_ascii_is_space_n(const char *s, size_t n)
{
	const unsigned char *pu;
	size_t i;

	if (n == 0u) {
		return 1;
	}
	if (s == NULL) {
		return 0;
	}

	pu = (const unsigned char *)s;
	for (i = 0u; i < n; i++) {
		if (!b660_is_space(pu[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_is_space_n(const char *s, size_t n)
    __attribute__((alias("gj_ascii_is_space_n")));
