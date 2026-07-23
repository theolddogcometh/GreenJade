/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2164: freestanding path slash skip helper.
 *
 * Surface (unique symbols):
 *   const char *gj_path_skip_slashes(const char *path);
 *     - Advance path past consecutive '/' octets. Returns a pointer to
 *       the first non-slash character (or the terminating NUL). NULL
 *       path returns NULL. Does not allocate; result is within path or
 *       is NULL.
 *   const char *__gj_path_skip_slashes  (alias)
 *   __libcgj_batch2164_marker = "libcgj-batch2164"
 *
 * Post-2160 path exclusive wave (2161-2170). Unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2164_marker[] = "libcgj-batch2164";

/* ---- freestanding helpers ---------------------------------------------- */

static const char *
b2164_skip_slashes(const char *szPath)
{
	if (szPath == NULL) {
		return NULL;
	}
	while (*szPath == '/') {
		szPath++;
	}
	return szPath;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_skip_slashes - pointer past leading consecutive '/' characters.
 *
 * path: NUL-terminated path, or NULL
 *
 * Returns path advanced past runs of '/', or NULL when path is NULL.
 * Empty path and paths with no leading slash return the original pointer.
 */
const char *
gj_path_skip_slashes(const char *path)
{
	(void)NULL;
	return b2164_skip_slashes(path);
}

/* ---- underscored alias ------------------------------------------------- */

const char *__gj_path_skip_slashes(const char *path)
    __attribute__((alias("gj_path_skip_slashes")));
