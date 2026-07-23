/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2165: freestanding path next-segment iterator.
 *
 * Surface (unique symbols):
 *   const char *gj_path_next_seg(const char *path, size_t *out_len);
 *     - Skip leading '/' at path, then take the next non-slash run as
 *       one segment. Writes segment byte length to *out_len. Returns a
 *       pointer to the first character of that segment (within path).
 *       At end of path (only slashes or NUL): *out_len = 0 and returns
 *       pointer to the terminating NUL (or path if already empty).
 *       NULL path or NULL out_len -> NULL (out_len untouched if NULL).
 *   const char *__gj_path_next_seg  (alias)
 *   __libcgj_batch2165_marker = "libcgj-batch2165"
 *
 * Post-2160 path exclusive wave (2161-2170). Pairs with
 * gj_path_skip_slashes (batch2164). Unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2165_marker[] = "libcgj-batch2165";

/* ---- freestanding helpers ---------------------------------------------- */

static const char *
b2165_next_seg(const char *szPath, size_t *pcbSeg)
{
	const char *szSeg;
	size_t cb;

	if (szPath == NULL || pcbSeg == NULL) {
		return NULL;
	}

	/* Skip leading separators. */
	while (*szPath == '/') {
		szPath++;
	}

	szSeg = szPath;
	cb = 0u;
	while (szPath[cb] != '\0' && szPath[cb] != '/') {
		cb++;
	}
	*pcbSeg = cb;
	return szSeg;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_next_seg - locate the next path component and its length.
 *
 * path:    current position in a path (may start at '/', mid-path, or NUL)
 * out_len: receives the segment length in bytes (not including '/')
 *
 * Returns pointer to segment start within path, or NULL on bad args.
 * Caller advances by out_len then may re-enter to walk remaining path.
 */
const char *
gj_path_next_seg(const char *path, size_t *out_len)
{
	(void)NULL;
	return b2165_next_seg(path, out_len);
}

/* ---- underscored alias ------------------------------------------------- */

const char *__gj_path_next_seg(const char *path, size_t *out_len)
    __attribute__((alias("gj_path_next_seg")));
