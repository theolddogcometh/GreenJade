/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6318: generic shader_cache path component stub.
 *
 * Surface (unique symbols):
 *   int gj_path_has_shader_cache_6318(const char *path);
 *     — Return 1 if path contains a full path component "shader_cache"
 *       (underscore form used by several drivers/tooling layouts), else 0.
 *       NULL / no match → 0. Case-sensitive; '/' is the only separator.
 *   int __gj_path_has_shader_cache_6318  (alias)
 *   __libcgj_batch6318_marker = "libcgj-batch6318"
 *
 * Shader cache path stubs exclusive CREATE-ONLY (6311-6320).
 * Unique gj_path_has_shader_cache_6318 surface only; no multi-def.
 * Distinct from "shadercache" (6311) and mesa_shader_cache (6312).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6318_marker[] = "libcgj-batch6318";

/* Needle: full path component "shader_cache" (underscore form). */
static const char b6318_needle[] = "shader_cache";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b6318_len(const char *sz)
{
	size_t n;

	n = 0;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static int
b6318_comp_eq(const char *p)
{
	size_t i;
	size_t nNeedle;

	nNeedle = b6318_len(b6318_needle);
	for (i = 0; i < nNeedle; i++) {
		if (p[i] != b6318_needle[i]) {
			return 0;
		}
	}
	if (p[nNeedle] != '\0' && p[nNeedle] != '/') {
		return 0;
	}
	return 1;
}

static int
b6318_has_comp(const char *szPath)
{
	const char *p;

	p = szPath;
	if (b6318_comp_eq(p)) {
		return 1;
	}
	while (*p != '\0') {
		if (*p == '/') {
			p++;
			if (*p == '\0') {
				break;
			}
			if (b6318_comp_eq(p)) {
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
 * gj_path_has_shader_cache_6318 — 1 iff path has a "shader_cache" component.
 *
 * Examples:
 *   ".../.cache/shader_cache/..." → 1
 *   "shader_cache"                → 1
 *   ".../shadercache/..."         → 0  (no underscore)
 *   NULL                          → 0
 */
int
gj_path_has_shader_cache_6318(const char *szPath)
{
	(void)sizeof(NULL);
	if (szPath == NULL) {
		return 0;
	}
	return b6318_has_comp(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_has_shader_cache_6318(const char *szPath)
    __attribute__((alias("gj_path_has_shader_cache_6318")));
