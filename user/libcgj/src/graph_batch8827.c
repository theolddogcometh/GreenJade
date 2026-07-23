/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8827: cgroup path form probe stub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cgroup_path_ok_u_8827(const char *path);
 *     - Return 1 if path is a non-NULL absolute Unix path that begins
 *       with '/', has at least one body octet after the leading slash,
 *       and fits a 4096-octet scan budget without an embedded NUL mid-
 *       check overflow. NULL / empty / relative / lone "/" → 0.
 *       Probe only — does not open cgroupfs or call libc.
 *   uint32_t __gj_cgroup_path_ok_u_8827  (alias)
 *   __libcgj_batch8827_marker = "libcgj-batch8827"
 *
 * Exclusive continuum CREATE-ONLY (8821-8830: cgroup/controller id
 * stubs — cpu_id, memory_id, io_id, pids_id, cpuset_id, v2_ready_u,
 * path_ok_u, fd_ok_u, ready_u, batch_id_8830). Unique
 * gj_cgroup_path_ok_u_8827 surface only; no multi-def. Distinct from
 * gj_rootfs_path_ok (batch2363) and gj_shm_name_ok_u_8716. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8827_marker[] = "libcgj-batch8827";

/* Inclusive scan budget for cgroup path octets (PATH_MAX-ish). */
#define B8827_SCAN_MAX  4096u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if szPath is a soft absolute cgroup path shape:
 *   '/' + non-empty body, NUL-terminated within B8827_SCAN_MAX.
 */
static uint32_t
b8827_path_ok(const char *szPath)
{
	size_t i;

	if (szPath == NULL || szPath[0] != '/') {
		return 0u;
	}
	/* Lone "/" is not a useful cgroup node path for this probe. */
	if (szPath[1] == '\0') {
		return 0u;
	}
	i = 1u;
	while (i < B8827_SCAN_MAX && szPath[i] != '\0') {
		i++;
	}
	/* Over-budget without NUL: reject (unbounded / truncated). */
	if (i >= B8827_SCAN_MAX && szPath[i] != '\0') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cgroup_path_ok_u_8827 - 1 if path is a soft absolute cgroup path.
 *
 * path: NUL-terminated candidate path (may be NULL)
 *
 * Probe stub only — no openat / cgroupfs. No parent wires.
 */
uint32_t
gj_cgroup_path_ok_u_8827(const char *szPath)
{
	(void)NULL;
	return b8827_path_ok(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cgroup_path_ok_u_8827(const char *szPath)
    __attribute__((alias("gj_cgroup_path_ok_u_8827")));
