/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1137: TOML basic-string quote necessity.
 *
 * Surface (unique symbols):
 *   int gj_toml_needs_quote(const char *s, size_t n);
 *     — Return 1 if the n-byte sequence s cannot be a bare TOML key
 *       (must be quoted). Bare keys are 1+ of A-Za-z0-9_- only.
 *       Empty (n==0), NULL with n>0, or any other octet → 1.
 *       Valid bare sequence → 0.
 *   int __gj_toml_needs_quote  (alias)
 *   __libcgj_batch1137_marker = "libcgj-batch1137"
 *
 * Distinct from gj_toml_get_* parsers — unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1137_marker[] = "libcgj-batch1137";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_toml_needs_quote — 1 if s[0..n) is not a bare TOML key.
 */
int
gj_toml_needs_quote(const char *sz, size_t cN)
{
	size_t i;
	unsigned char uCh;

	if (cN == 0u) {
		return 1;
	}
	if (sz == NULL) {
		return 1;
	}

	for (i = 0u; i < cN; i++) {
		uCh = (unsigned char)sz[i];
		if ((uCh >= 'A' && uCh <= 'Z') ||
		    (uCh >= 'a' && uCh <= 'z') ||
		    (uCh >= '0' && uCh <= '9') ||
		    uCh == '_' || uCh == '-') {
			continue;
		}
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_toml_needs_quote(const char *sz, size_t cN)
    __attribute__((alias("gj_toml_needs_quote")));
