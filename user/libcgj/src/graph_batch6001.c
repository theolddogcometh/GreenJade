/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6001: bar3 install path absolute-ok deepen.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_path_abs_ok_6001(const char *path);
 *     - Return 1 if path is absolute with at least one non-empty
 *       segment beyond the leading '/', else 0. Soft pure-data
 *       install-root shape check for the post-6000 bar3 install path
 *       deepen wave. NULL or relative paths fail.
 *   uint32_t __gj_bar3_install_path_abs_ok_6001  (alias)
 *   __libcgj_batch6001_marker = "libcgj-batch6001"
 *
 * Exclusive continuum CREATE-ONLY (6001-6010: post-6000 bar3 install
 * path deepen — abs_ok_6001, seg_count_6002, depth_min_6003,
 * depth_max_6004, depth_ok_6005, leaf_len_6006, deepen_score_6007,
 * deepen_threshold_6008, deepen_pass_6009, deepen_ready_6010). Unique
 * gj_bar3_install_path_abs_ok_6001 surface only; no multi-def.
 * Distinct from gj_steam_runtime_root_ok_5409 and
 * gj_path_is_root_p (batch2163). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6001_marker[] = "libcgj-batch6001";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6001_abs_ok(const char *szPath)
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
 * gj_bar3_install_path_abs_ok_6001 - absolute non-root install path ok.
 *
 * path: NUL-terminated install path (NULL -> 0)
 *
 * Returns 1 when path starts with '/' and has at least one non-empty
 * segment, else 0. Soft pure-data shape check; does not walk filesystems
 * or call libc. No parent wires.
 */
uint32_t
gj_bar3_install_path_abs_ok_6001(const char *szPath)
{
	(void)NULL;
	return b6001_abs_ok(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_path_abs_ok_6001(const char *szPath)
    __attribute__((alias("gj_bar3_install_path_abs_ok_6001")));
