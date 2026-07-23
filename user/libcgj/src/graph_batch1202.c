/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1202: n-bounded graph (no-space) predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_is_graph_n(const char *s, size_t n);
 *     — Return 1 (OK) if every octet in s[0..n) is ASCII graph
 *       (0x21..0x7E; printable excluding space); else 0 (FAIL).
 *       NULL with n > 0 → 0. n == 0 → 1 (vacuous). No NUL required.
 *   int __gj_str_is_graph_n  (alias)
 *   __libcgj_batch1202_marker = "libcgj-batch1202"
 *
 * Distinct from isgraph / gj_str_is_printable_n — unique name; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1202_marker[] = "libcgj-batch1202";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_graph_n — 1 if s[0..n) are all graph chars (no space).
 */
int
gj_str_is_graph_n(const char *sz, size_t cbN)
{
	const unsigned char *p;
	size_t i;

	if (cbN == 0u) {
		return 1;
	}
	if (sz == NULL) {
		return 0;
	}

	p = (const unsigned char *)sz;
	for (i = 0u; i < cbN; i++) {
		if (p[i] < 0x21u || p[i] > 0x7eu) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_graph_n(const char *sz, size_t cbN)
    __attribute__((alias("gj_str_is_graph_n")));
