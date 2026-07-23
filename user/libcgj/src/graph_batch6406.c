/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6406: bar3 install path leaf segment length.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_path_leaf_len_6406(const char *path);
 *     - Return the byte length of the final non-empty path segment
 *       (basename-style leaf), ignoring trailing '/'. NULL or no
 *       non-empty segments return 0. Soft pure-data leaf measure for
 *       the post-6400 bar3 deepen install/steam/top50 wave.
 *   uint32_t __gj_bar3_install_path_leaf_len_6406  (alias)
 *   __libcgj_batch6406_marker = "libcgj-batch6406"
 *
 * Exclusive continuum CREATE-ONLY (6401-6410: post-6400 bar3 deepen
 * install/steam/top50). Unique gj_bar3_install_path_leaf_len_6406
 * surface only; no multi-def. Distinct from
 * gj_bar3_install_path_leaf_len_6206 (batch6206),
 * gj_bar3_install_path_leaf_len_6006 (batch6006), and
 * gj_path_stem_start (batch2169). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6406_marker[] = "libcgj-batch6406";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6406_leaf_len(const char *szPath)
{
	size_t iEnd;
	size_t i;
	uint32_t uLen;

	if (szPath == NULL || szPath[0] == '\0') {
		return 0u;
	}
	/* Find string end. */
	iEnd = 0u;
	while (szPath[iEnd] != '\0') {
		iEnd++;
	}
	/* Trim trailing slashes (but not pure root alone). */
	while (iEnd > 0u && szPath[iEnd - 1u] == '/') {
		iEnd--;
	}
	if (iEnd == 0u) {
		return 0u;
	}
	/* Walk back to prior slash or start. */
	i = iEnd;
	while (i > 0u && szPath[i - 1u] != '/') {
		i--;
	}
	uLen = 0u;
	while (i < iEnd) {
		if (uLen < UINT32_MAX) {
			uLen++;
		}
		i++;
	}
	return uLen;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_path_leaf_len_6406 - final non-empty segment length.
 *
 * path: NUL-terminated install/steam/top50 product path (NULL -> 0)
 *
 * Returns leaf segment byte length after trimming trailing '/'. Soft
 * pure-data measure. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_install_path_leaf_len_6406(const char *szPath)
{
	(void)NULL;
	return b6406_leaf_len(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_path_leaf_len_6406(const char *szPath)
    __attribute__((alias("gj_bar3_install_path_leaf_len_6406")));
