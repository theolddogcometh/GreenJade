/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6319: NVIDIA GLCache path component stub.
 *
 * Surface (unique symbols):
 *   int gj_path_has_glcache_6319(const char *path);
 *     — Return 1 if path contains a full path component "GLCache"
 *       (NVIDIA OpenGL binary shader cache directory name), else 0.
 *       NULL / no match → 0. Case-sensitive; '/' is the only separator.
 *   int __gj_path_has_glcache_6319  (alias)
 *   __libcgj_batch6319_marker = "libcgj-batch6319"
 *
 * Shader cache path stubs exclusive CREATE-ONLY (6311-6320).
 * Unique gj_path_has_glcache_6319 surface only; no multi-def.
 * Distinct from mesa_shader_cache (6312) and shader_cache (6318).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6319_marker[] = "libcgj-batch6319";

/* Needle: full path component "GLCache" (NVIDIA GL binary cache). */
static const char b6319_needle[] = "GLCache";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b6319_len(const char *sz)
{
	size_t n;

	n = 0;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static int
b6319_comp_eq(const char *p)
{
	size_t i;
	size_t nNeedle;

	nNeedle = b6319_len(b6319_needle);
	for (i = 0; i < nNeedle; i++) {
		if (p[i] != b6319_needle[i]) {
			return 0;
		}
	}
	if (p[nNeedle] != '\0' && p[nNeedle] != '/') {
		return 0;
	}
	return 1;
}

static int
b6319_has_comp(const char *szPath)
{
	const char *p;

	p = szPath;
	if (b6319_comp_eq(p)) {
		return 1;
	}
	while (*p != '\0') {
		if (*p == '/') {
			p++;
			if (*p == '\0') {
				break;
			}
			if (b6319_comp_eq(p)) {
				return 1;
			}
		} else {
			p++;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_has_glcache_6319 — 1 iff path has a "GLCache" component.
 *
 * Examples:
 *   ".../nvidia/GLCache/..." → 1
 *   "GLCache"                → 1
 *   ".../glcache/..."        → 0  (case-sensitive)
 *   NULL                     → 0
 */
int
gj_path_has_glcache_6319(const char *szPath)
{
	(void)sizeof(NULL);
	if (szPath == NULL) {
		return 0;
	}
	return b6319_has_comp(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_has_glcache_6319(const char *szPath)
    __attribute__((alias("gj_path_has_glcache_6319")));
