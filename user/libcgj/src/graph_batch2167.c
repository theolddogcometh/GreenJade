/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2167: freestanding path dot-normalize length.
 *
 * Surface (unique symbols):
 *   size_t gj_path_norm_dot_len(const char *path);
 *     - Return the byte length (excluding NUL) of path after a lexical
 *       "."-only clean: drop empty and single-dot segments, collapse
 *       repeated '/', keep ".." segments as ordinary components, keep
 *       absolute leading '/', and map an empty relative result to 1
 *       (length of "."). NULL path -> 1 (treat as ".").
 *   size_t __gj_path_norm_dot_len  (alias)
 *   __libcgj_batch2167_marker = "libcgj-batch2167"
 *
 * Post-2160 path exclusive wave (2161-2170). Distinct from
 * gj_path_normalize (batch263 full clean with "..") - length-only,
 * dot-only surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2167_marker[] = "libcgj-batch2167";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Length of a "."-cleaned path. Does not resolve ".." (kept as text).
 * Mirrors a length-only subset of path.Clean without climbing parents.
 */
static size_t
b2167_norm_dot_len(const char *szPath)
{
	size_t i;
	size_t iStart;
	size_t cbSeg;
	size_t cbOut;
	int fAbs;
	int fWrote;

	if (szPath == NULL || szPath[0] == '\0') {
		return 1u; /* "." */
	}

	fAbs = (szPath[0] == '/') ? 1 : 0;
	cbOut = 0u;
	fWrote = 0;
	iStart = 0u;

	for (i = 0u; ; i++) {
		if (szPath[i] == '/' || szPath[i] == '\0') {
			cbSeg = i - iStart;

			/* Skip empty (from //) and single-dot segments. */
			if (cbSeg == 0u) {
				/* empty */
			} else if (cbSeg == 1u && szPath[iStart] == '.') {
				/* drop "." */
			} else {
				/* Emit '/' before later relative segments,
				 * or between absolute components. */
				if (fWrote) {
					cbOut++; /* separator */
				} else if (fAbs) {
					cbOut++; /* leading root slash */
				}
				cbOut += cbSeg;
				fWrote = 1;
			}

			if (szPath[i] == '\0') {
				break;
			}
			iStart = i + 1u;
		}
	}

	if (!fWrote) {
		/* Absolute with only dots/empties -> "/"; relative -> "." */
		return 1u;
	}
	return cbOut;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_norm_dot_len - content length after dropping "." segments.
 *
 * path: NUL-terminated path, or NULL
 *
 * Returns length excluding NUL. Empty relative results count as 1 (".").
 * Does not call libc and does not write through path.
 */
size_t
gj_path_norm_dot_len(const char *path)
{
	(void)NULL;
	return b2167_norm_dot_len(path);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_norm_dot_len(const char *path)
    __attribute__((alias("gj_path_norm_dot_len")));
