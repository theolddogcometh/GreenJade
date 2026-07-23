/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6313: Mesa mesa_shader_cache_db path component.
 *
 * Surface (unique symbols):
 *   int gj_path_has_mesa_shader_cache_db_6313(const char *path);
 *     — Return 1 if path contains a full path component
 *       "mesa_shader_cache_db" (Mesa multi-file DB shader cache root),
 *       else 0. NULL / no match → 0. Case-sensitive; '/' only separator.
 *   int __gj_path_has_mesa_shader_cache_db_6313  (alias)
 *   __libcgj_batch6313_marker = "libcgj-batch6313"
 *
 * Shader cache path stubs exclusive CREATE-ONLY (6311-6320).
 * Unique gj_path_has_mesa_shader_cache_db_6313 surface only; no multi-def.
 * Distinct from gj_path_has_mesa_shader_cache_6312. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6313_marker[] = "libcgj-batch6313";

/* Needle: full path component "mesa_shader_cache_db". */
static const char b6313_needle[] = "mesa_shader_cache_db";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b6313_len(const char *sz)
{
	size_t n;

	n = 0;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static int
b6313_comp_eq(const char *p)
{
	size_t i;
	size_t nNeedle;

	nNeedle = b6313_len(b6313_needle);
	for (i = 0; i < nNeedle; i++) {
		if (p[i] != b6313_needle[i]) {
			return 0;
		}
	}
	if (p[nNeedle] != '\0' && p[nNeedle] != '/') {
		return 0;
	}
	return 1;
}

static int
b6313_has_comp(const char *szPath)
{
	const char *p;

	p = szPath;
	if (b6313_comp_eq(p)) {
		return 1;
	}
	while (*p != '\0') {
		if (*p == '/') {
			p++;
			if (*p == '\0') {
				break;
			}
			if (b6313_comp_eq(p)) {
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
 * gj_path_has_mesa_shader_cache_db_6313 — 1 iff "mesa_shader_cache_db".
 *
 * Examples:
 *   ".../.cache/mesa_shader_cache_db/..." → 1
 *   "mesa_shader_cache_db"                → 1
 *   ".../mesa_shader_cache/..."           → 0
 *   NULL                                  → 0
 */
int
gj_path_has_mesa_shader_cache_db_6313(const char *szPath)
{
	(void)sizeof(NULL);
	if (szPath == NULL) {
		return 0;
	}
	return b6313_has_comp(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_has_mesa_shader_cache_db_6313(const char *szPath)
    __attribute__((alias("gj_path_has_mesa_shader_cache_db_6313")));
