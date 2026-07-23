/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8828: cgroup fd soft validity stub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cgroup_fd_ok_u_8828(int32_t fd);
 *     - Return 1 if fd >= 0 (soft non-negative fd check); else 0.
 *   uint32_t __gj_cgroup_fd_ok_u_8828  (alias)
 *   __libcgj_batch8828_marker = "libcgj-batch8828"
 *
 * Exclusive continuum CREATE-ONLY (8821-8830: cgroup/controller id
 * stubs — cpu_id, memory_id, io_id, pids_id, cpuset_id, v2_ready_u,
 * path_ok_u, fd_ok_u, ready_u, batch_id_8830). Unique
 * gj_cgroup_fd_ok_u_8828 surface only; no multi-def. Distinct from
 * gj_cfr_fd_ok_6925. Soft pure-data gate only; no fstat or cgroup
 * syscalls. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8828_marker[] = "libcgj-batch8828";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8828_fd_ok(int32_t nFd)
{
	return (nFd >= 0) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cgroup_fd_ok_u_8828 - soft non-negative fd check for cgroup ops.
 *
 * fd: soft file-descriptor integer (cgroup directory / control file)
 *
 * Returns 1 when fd is non-negative, else 0. Pure-data gate; does not
 * call fstat or open cgroupfs. No parent wires.
 */
uint32_t
gj_cgroup_fd_ok_u_8828(int32_t nFd)
{
	(void)NULL;
	return b8828_fd_ok(nFd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cgroup_fd_ok_u_8828(int32_t nFd)
    __attribute__((alias("gj_cgroup_fd_ok_u_8828")));
