/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2166: freestanding two-component join length.
 *
 * Surface (unique symbols):
 *   size_t gj_path_join2_len(const char *a, const char *b);
 *     - Return the byte length (excluding NUL) of the path that
 *       gj_path_join2 (batch478) would write for (a, b). NULL a or b
 *       yields 0. Empty a -> len(b); empty b -> len(a). If a already
 *       ends with '/', no extra separator is counted; otherwise one
 *       '/' is counted between a and b when both are non-empty.
 *   size_t __gj_path_join2_len  (alias)
 *   __libcgj_batch2166_marker = "libcgj-batch2166"
 *
 * Post-2160 path exclusive wave (2161-2170). Distinct from
 * gj_path_join2 (buffer form) - length-only surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2166_marker[] = "libcgj-batch2166";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b2166_len(const char *sz)
{
	size_t n = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static size_t
b2166_join2_len(const char *szA, const char *szB)
{
	size_t cbA;
	size_t cbB;
	int fSlash;

	if (szA == NULL || szB == NULL) {
		return 0u;
	}

	cbA = b2166_len(szA);
	cbB = b2166_len(szB);

	if (cbA == 0u) {
		return cbB;
	}
	if (cbB == 0u) {
		return cbA;
	}

	fSlash = (szA[cbA - 1u] == '/') ? 1 : 0;
	return cbA + cbB + (fSlash ? 0u : 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_join2_len - byte length of join2(a,b) without writing.
 *
 * a, b: path components (NULL either -> 0)
 *
 * Returns content length excluding the terminating NUL.
 */
size_t
gj_path_join2_len(const char *a, const char *b)
{
	(void)NULL;
	return b2166_join2_len(a, b);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_join2_len(const char *a, const char *b)
    __attribute__((alias("gj_path_join2_len")));
