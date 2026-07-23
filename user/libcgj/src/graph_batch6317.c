/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6317: Vulkan pipeline_cache path component stub.
 *
 * Surface (unique symbols):
 *   int gj_path_has_pipeline_cache_6317(const char *path);
 *     — Return 1 if path contains a full path component "pipeline_cache"
 *       (Vulkan/driver pipeline binary cache directory), else 0.
 *       NULL / no match → 0. Case-sensitive; '/' is the only separator.
 *   int __gj_path_has_pipeline_cache_6317  (alias)
 *   __libcgj_batch6317_marker = "libcgj-batch6317"
 *
 * Shader cache path stubs exclusive CREATE-ONLY (6311-6320).
 * Unique gj_path_has_pipeline_cache_6317 surface only; no multi-def.
 * Distinct from dxvk_cache (6316) and shader_cache (6318). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6317_marker[] = "libcgj-batch6317";

/* Needle: full path component "pipeline_cache". */
static const char b6317_needle[] = "pipeline_cache";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b6317_len(const char *sz)
{
	size_t n;

	n = 0;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static int
b6317_comp_eq(const char *p)
{
	size_t i;
	size_t nNeedle;

	nNeedle = b6317_len(b6317_needle);
	for (i = 0; i < nNeedle; i++) {
		if (p[i] != b6317_needle[i]) {
			return 0;
		}
	}
	if (p[nNeedle] != '\0' && p[nNeedle] != '/') {
		return 0;
	}
	return 1;
}

static int
b6317_has_comp(const char *szPath)
{
	const char *p;

	p = szPath;
	if (b6317_comp_eq(p)) {
		return 1;
	}
	while (*p != '\0') {
		if (*p == '/') {
			p++;
			if (*p == '\0') {
				break;
			}
			if (b6317_comp_eq(p)) {
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
 * gj_path_has_pipeline_cache_6317 — 1 iff path has "pipeline_cache".
 *
 * Examples:
 *   ".../var/pipeline_cache/app" → 1
 *   "pipeline_cache"             → 1
 *   ".../pipeline_cacheX/..."    → 0
 *   NULL                         → 0
 */
int
gj_path_has_pipeline_cache_6317(const char *szPath)
{
	(void)sizeof(NULL);
	if (szPath == NULL) {
		return 0;
	}
	return b6317_has_comp(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_has_pipeline_cache_6317(const char *szPath)
    __attribute__((alias("gj_path_has_pipeline_cache_6317")));
