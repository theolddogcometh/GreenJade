/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch352: capacity-bounded strcat (safe append).
 *
 * Surface (unique symbols):
 *   int gj_str_cat_cap(char *dst, size_t cap, const char *src);
 *     — Append NUL-terminated src onto dst without writing past dst[cap).
 *       dst is a caller-owned buffer of capacity cap (including space for
 *       the terminating NUL). Locates the existing NUL in dst within
 *       [0, cap), then copies src after it. Always leaves a terminator
 *       when the append succeeds. Returns 0 if the entire src was
 *       appended (or src was empty / NULL); -1 on bad args, missing
 *       terminator in dst, or insufficient room for the full append.
 *       Failure with a valid terminator present leaves dst unchanged
 *       (all-or-nothing; no partial append).
 *   int __gj_str_cat_cap  (alias)
 *   __libcgj_batch352_marker = "libcgj-batch352"
 *
 * Distinct from string.c strlcat / strcat / strncat (those are size_t or
 * char * returns and may truncate). This surface is all-or-nothing:
 * success means the whole source was concatenated; failure means dst was
 * not modified (when a valid terminator existed inside the capacity).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch352_marker[] = "libcgj-batch352";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Length of sz within the first cbMax bytes, not counting a terminator.
 * If no NUL appears in [0, cbMax), returns cbMax (unterminated).
 * sz == NULL → 0.
 */
static size_t
b352_strnlen(const char *sz, size_t cbMax)
{
	size_t i;

	if (sz == NULL) {
		return 0u;
	}
	for (i = 0u; i < cbMax; i++) {
		if (sz[i] == '\0') {
			return i;
		}
	}
	return cbMax;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_cat_cap — append src to dst within capacity cap (including NUL).
 *
 * dst: buffer holding a NUL-terminated string; capacity is cap bytes
 * cap: total size of dst (must be > 0 for any write)
 * src: NUL-terminated source to append; NULL treated as empty string
 *
 * Returns 0 on full successful append, -1 on failure:
 *   - dst == NULL
 *   - cap == 0
 *   - dst has no NUL in [0, cap) (unterminated / truncated view)
 *   - remaining room after existing content is insufficient for src + NUL
 *
 * On -1 with a valid terminator present, dst is left unchanged.
 * On success, dst ends with a single NUL and contains the concatenation.
 */
int
gj_str_cat_cap(char *dst, size_t cap, const char *src)
{
	size_t cbDst;
	size_t cbSrc;
	size_t cbRoom;
	size_t i;

	if (dst == NULL || cap == 0u) {
		return -1;
	}

	/* Existing content must already be terminated inside capacity. */
	cbDst = b352_strnlen(dst, cap);
	if (cbDst == cap) {
		/* No NUL found — cannot safely append. */
		return -1;
	}

	/* Measure source (NULL → empty). */
	if (src == NULL) {
		cbSrc = 0u;
	} else {
		/*
		 * Bound source scan by residual room + 1 so a huge/unterminated
		 * src cannot walk forever; residual slots after the existing
		 * NUL are (cap - cbDst - 1) payload bytes.
		 */
		cbRoom = cap - cbDst - 1u;
		cbSrc = b352_strnlen(src, cbRoom + 1u);
	}

	/* Need cbSrc payload bytes plus the final NUL already reserved. */
	if (cbSrc > (cap - cbDst - 1u)) {
		/* Insufficient capacity for full append; leave dst alone. */
		return -1;
	}

	for (i = 0u; i < cbSrc; i++) {
		dst[cbDst + i] = src[i];
	}
	dst[cbDst + cbSrc] = '\0';
	return 0;
}

int __gj_str_cat_cap(char *dst, size_t cap, const char *src)
    __attribute__((alias("gj_str_cat_cap")));
