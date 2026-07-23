/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch263: URL/path normalize (POSIX-ish).
 *
 * Surface (unique symbols):
 *   int gj_path_normalize(const char *in, char *out, size_t out_cap);
 *     — Collapse repeated '/' separators, drop '.' segments, and resolve
 *       '..' segments (POSIX path.Clean / normpath style). Result is
 *       NUL-terminated in out. Requires out_cap >= 2.
 *       Stack-bounded: input length (excluding NUL) must be <= 512.
 *       Returns 0 on success, -1 on error (NULL args, short cap, or
 *       input longer than 512). On error out is undefined.
 *   __gj_path_normalize  (alias)
 *   __libcgj_batch263_marker = "libcgj-batch263"
 *
 * Semantics (clean-room, freestanding):
 *   ""           → "."
 *   "."          → "."
 *   ".."         → ".."
 *   "/"          → "/"
 *   "/a//b/./c/../d" → "/a/b/d"
 *   "a/b/../c"   → "a/c"
 *   "/../a"      → "/a"   (absolute cannot climb above root)
 *   "../a"       → "../a" (relative keeps leading ..)
 *   trailing '/' stripped except for root "/".
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party path library source copied. Distinct from batch149.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch263_marker[] = "libcgj-batch263";

/* Maximum accepted input length (excluding trailing NUL). Stack work buf. */
#define B263_MAX_IN 512u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b263_strlen_cap — length of NUL-terminated in, capped at nMax+1.
 * Returns length in 0..nMax, or (size_t)-1 if longer than nMax.
 */
static size_t
b263_strlen_cap(const char *sz, size_t nMax)
{
	size_t n;

	for (n = 0u; n <= nMax; n++) {
		if (sz[n] == '\0') {
			return n;
		}
	}
	return (size_t)-1;
}

/* Append one byte to stack buf; leave room for final NUL. Returns 0 / -1. */
static int
b263_putc(char *buf, size_t cap, size_t *pw, char ch)
{
	if (*pw + 1u >= cap) {
		return -1;
	}
	buf[(*pw)++] = ch;
	return 0;
}

/* Append n bytes from src. Returns 0 / -1. */
static int
b263_putn(char *buf, size_t cap, size_t *pw, const char *src, size_t n)
{
	size_t i;

	if (n > 0u && src == NULL) {
		return -1;
	}
	if (*pw + n + 1u > cap) {
		return -1;
	}
	for (i = 0u; i < n; i++) {
		buf[(*pw)++] = src[i];
	}
	return 0;
}

/*
 * b263_clean — POSIX-ish path clean into stack buffer of capacity cap.
 *
 * rooted: path begins with '/'. buf[0..*pw) grows the cleaned form.
 * dotdot: floor index — ".." may not pop below it (root slash, or the
 * end of a leading relative "../../" prefix). Same invariant as Go
 * path.Clean / Plan 9 lexically clean paths.
 *
 * empty relative result is left with *pw == 0 (caller writes ".").
 * empty absolute result keeps a single '/'.
 *
 * Returns 0 on success, -1 if the stack buffer is too small (should not
 * happen for nIn <= B263_MAX_IN with cap == B263_MAX_IN+1).
 */
static int
b263_clean(const char *in, size_t nIn, char *buf, size_t cap, size_t *pw)
{
	size_t r = 0u;
	size_t w = 0u;
	size_t dotdot = 0u;
	int rooted;

	rooted = (nIn > 0u && in[0] == '/') ? 1 : 0;

	if (rooted) {
		if (b263_putc(buf, cap, &w, '/') != 0) {
			return -1;
		}
		/* Skip all leading slashes (collapse // at root). */
		while (r < nIn && in[r] == '/') {
			r++;
		}
		dotdot = 1u;
	}

	while (r < nIn) {
		/* Skip separator runs between components (collapse //). */
		if (in[r] == '/') {
			r++;
			continue;
		}

		/* "." segment → drop */
		if (in[r] == '.' &&
		    (r + 1u >= nIn || in[r + 1u] == '/')) {
			r++;
			continue;
		}

		/* ".." segment */
		if (in[r] == '.' && r + 1u < nIn && in[r + 1u] == '.' &&
		    (r + 2u >= nIn || in[r + 2u] == '/')) {
			r += 2u;
			if (w > dotdot) {
				/*
				 * Pop last component (not past leading .. / root).
				 * After w--, walk back while buf[w] is not '/';
				 * final w is exclusive end (lands on prior '/'
				 * or at dotdot), matching Go path.Clean.
				 */
				w--;
				while (w > dotdot && buf[w] != '/') {
					w--;
				}
			} else if (!rooted) {
				/* Relative: cannot climb; append another "..". */
				if (w > 0u) {
					if (b263_putc(buf, cap, &w, '/') != 0) {
						return -1;
					}
				}
				if (b263_putc(buf, cap, &w, '.') != 0) {
					return -1;
				}
				if (b263_putc(buf, cap, &w, '.') != 0) {
					return -1;
				}
				dotdot = w;
			}
			/* Absolute at / with w == dotdot: ".." is a no-op. */
			continue;
		}

		/* Ordinary component: optional '/', then bytes until '/' or end. */
		if ((rooted && w != 1u) || (!rooted && w != 0u)) {
			if (b263_putc(buf, cap, &w, '/') != 0) {
				return -1;
			}
		}
		{
			size_t start = r;

			while (r < nIn && in[r] != '/') {
				r++;
			}
			if (b263_putn(buf, cap, &w, in + start, r - start) != 0) {
				return -1;
			}
		}
	}

	*pw = w;
	return 0;
}

/* Copy n bytes + NUL into out when out_cap is sufficient. Returns 0 / -1. */
static int
b263_emit(char *out, size_t out_cap, const char *src, size_t n)
{
	size_t i;

	if (n + 1u > out_cap) {
		return -1;
	}
	for (i = 0u; i < n; i++) {
		out[i] = src[i];
	}
	out[n] = '\0';
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_normalize — collapse //, resolve . and .., write to out.
 *
 * in:       NUL-terminated path (may be empty). Length must be <= 512.
 * out:      destination buffer; must be non-NULL.
 * out_cap:  capacity of out including room for NUL; must be >= 2.
 *
 * Returns 0 on success with a NUL-terminated cleaned path in out.
 * Returns -1 if in/out is NULL, out_cap < 2, input longer than 512, or
 * the cleaned result (plus NUL) does not fit in out_cap.
 */
int
gj_path_normalize(const char *in, char *out, size_t out_cap)
{
	char buf[B263_MAX_IN + 1u];
	size_t nIn;
	size_t w;

	if (in == NULL || out == NULL || out_cap < 2u) {
		return -1;
	}

	nIn = b263_strlen_cap(in, B263_MAX_IN);
	if (nIn == (size_t)-1) {
		return -1;
	}

	w = 0u;
	if (b263_clean(in, nIn, buf, sizeof(buf), &w) != 0) {
		return -1;
	}

	/* Empty relative clean → "." */
	if (w == 0u) {
		return b263_emit(out, out_cap, ".", 1u);
	}

	return b263_emit(out, out_cap, buf, w);
}

int __gj_path_normalize(const char *in, char *out, size_t out_cap)
    __attribute__((alias("gj_path_normalize")));
