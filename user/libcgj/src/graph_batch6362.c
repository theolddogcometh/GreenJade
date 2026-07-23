/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6362: cloud save path absolute-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cloud_save_path_abs_ok_6362(const char *path);
 *     - Return 1 if path is absolute with at least one non-empty
 *       segment beyond the leading '/', else 0. Soft pure-data
 *       cloud save root shape check. NULL or relative paths fail.
 *   uint32_t __gj_cloud_save_path_abs_ok_6362  (alias)
 *   __libcgj_batch6362_marker = "libcgj-batch6362"
 *
 * Exclusive continuum CREATE-ONLY (6361-6370: cloud save path stubs —
 * max_len_6361, abs_ok_6362, seg_count_6363, depth_ok_6364,
 * len_ok_6365, leaf_len_6366, leaf_ok_6367, score_6368, pack_6369,
 * batch_id / wave_ready_6370). Unique surface only; no multi-def.
 * Distinct from gj_bar3_install_path_abs_ok_6201. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6362_marker[] = "libcgj-batch6362";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6362_abs_ok(const char *szPath)
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
 * gj_cloud_save_path_abs_ok_6362 - absolute non-root cloud save path ok.
 *
 * path: NUL-terminated cloud save path (NULL -> 0)
 *
 * Returns 1 when path starts with '/' and has at least one non-empty
 * segment, else 0. Soft pure-data shape check; does not walk backends
 * or call libc. No parent wires.
 */
uint32_t
gj_cloud_save_path_abs_ok_6362(const char *szPath)
{
	(void)NULL;
	return b6362_abs_ok(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cloud_save_path_abs_ok_6362(const char *szPath)
    __attribute__((alias("gj_cloud_save_path_abs_ok_6362")));
