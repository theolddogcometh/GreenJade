/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6401: bar3 install path absolute-ok deepen.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_path_abs_ok_6401(const char *path);
 *     - Return 1 if path is absolute with at least one non-empty
 *       segment beyond the leading '/', else 0. Soft pure-data
 *       install-root shape check for the post-6400 bar3 deepen
 *       install/steam/top50 wave. NULL or relative paths fail.
 *   uint32_t __gj_bar3_install_path_abs_ok_6401  (alias)
 *   __libcgj_batch6401_marker = "libcgj-batch6401"
 *
 * Exclusive continuum CREATE-ONLY (6401-6410: post-6400 bar3 deepen
 * install/steam/top50 — abs_ok_6401, seg_count_6402, depth_min_6403,
 * depth_max_6404, depth_ok_6405, leaf_len_6406, deepen_score_6407,
 * deepen_threshold_6408, deepen_pass_6409, deepen_ready_6410). Unique
 * gj_bar3_install_path_abs_ok_6401 surface only; no multi-def.
 * Distinct from gj_bar3_install_path_abs_ok_6201 (batch6201),
 * gj_bar3_install_path_abs_ok_6001 (batch6001), and
 * gj_steam_runtime_root_ok_5409. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6401_marker[] = "libcgj-batch6401";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6401_abs_ok(const char *szPath)
{
	size_t i;

	if (szPath == NULL) {
		return 0u;
	}
	if (szPath[0] != '/') {
		return 0u;
	}
	/* Require at least one non-slash character after leading '/'. */
	i = 1u;
	while (szPath[i] == '/') {
		i++;
	}
	if (szPath[i] == '\0') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_path_abs_ok_6401 - absolute non-root install path ok.
 *
 * path: NUL-terminated install/steam/top50 product path (NULL -> 0)
 *
 * Returns 1 when path starts with '/' and has at least one non-empty
 * segment, else 0. Soft pure-data shape check; does not walk filesystems
 * or call libc. No parent wires.
 */
uint32_t
gj_bar3_install_path_abs_ok_6401(const char *szPath)
{
	(void)NULL;
	return b6401_abs_ok(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_path_abs_ok_6401(const char *szPath)
    __attribute__((alias("gj_bar3_install_path_abs_ok_6401")));
