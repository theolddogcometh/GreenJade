/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3483: freestanding join2 buffer-need (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_path_join2_need_u(const char *a, const char *b);
 *     - Bytes needed (including trailing NUL) for the path that
 *       gj_path_join2 (batch478) would write for (a, b). NULL a or b
 *       yields 0. Empty a -> len(b)+1; empty b -> len(a)+1. If a already
 *       ends with '/', no extra separator is counted; otherwise one '/'
 *       is counted between a and b when both are non-empty. Both empty
 *       yields 1 (NUL only).
 *   size_t __gj_path_join2_need_u  (alias)
 *   __libcgj_batch3483_marker = "libcgj-batch3483"
 *
 * Path/URI/MIME exclusive CREATE-ONLY wave (3481-3490). Distinct from
 * gj_path_join2_len (batch2166 content length), gj_path_join_need_u
 * (batch2828 NULL-as-empty), and gj_path_join2 (batch478 write). No
 * parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3483_marker[] = "libcgj-batch3483";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3483_len(const char *sz)
{
	size_t n;

	if (sz == NULL) {
		return 0u;
	}
	n = 0u;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/*
 * Bytes needed (incl. trailing NUL) for join2(a,b). NULL either -> 0.
 */
static size_t
b3483_join2_need(const char *szA, const char *szB)
{
	size_t cbA;
	size_t cbB;
	size_t nOut;
	int fSlash;

	if (szA == NULL || szB == NULL) {
		return 0u;
	}

	cbA = b3483_len(szA);
	cbB = b3483_len(szB);

	if (cbA == 0u && cbB == 0u) {
		return 1u; /* "" + NUL */
	}
	if (cbA == 0u) {
		return cbB + 1u;
	}
	if (cbB == 0u) {
		return cbA + 1u;
	}

	fSlash = (szA[cbA - 1u] == '/') ? 1 : 0;
	nOut = cbA + cbB;
	if (fSlash == 0) {
		nOut = nOut + 1u; /* inserted '/' */
	}
	return nOut + 1u; /* trailing NUL */
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_join2_need_u - bytes needed (incl. NUL) for join2(a,b).
 *
 * a, b: path components (NULL either -> 0)
 *
 * Examples (return includes trailing NUL):
 *   ("a","b")  -> 4  ("a/b" + NUL)
 *   ("a/","b") -> 4  ("a/b" + NUL)
 *   ("","")    -> 1  ("" + NUL)
 *   (NULL,"x") -> 0
 *   ("a","")   -> 2  ("a" + NUL)
 *
 * Length-only; does not write a buffer. Does not call libc.
 */
size_t
gj_path_join2_need_u(const char *szA, const char *szB)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b3483_join2_need(szA, szB);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_join2_need_u(const char *szA, const char *szB)
    __attribute__((alias("gj_path_join2_need_u")));
