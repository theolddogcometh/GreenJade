/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8825: cgroup cpuset controller id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cgroup_cpuset_id_8825(void);
 *     - Return soft cgroup cpuset controller catalog id (5).
 *   uint32_t __gj_cgroup_cpuset_id_8825  (alias)
 *   __libcgj_batch8825_marker = "libcgj-batch8825"
 *
 * Exclusive continuum CREATE-ONLY (8821-8830: cgroup/controller id
 * stubs — cpu_id, memory_id, io_id, pids_id, cpuset_id, v2_ready_u,
 * path_ok_u, fd_ok_u, ready_u, batch_id_8830). Unique
 * gj_cgroup_cpuset_id_8825 surface only; no multi-def. Catalog id only;
 * no cgroup syscalls. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8825_marker[] = "libcgj-batch8825";

/* Soft catalog ordinal for the cgroup cpuset controller. */
#define B8825_CGROUP_CPUSET_ID  5u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8825_cpuset_id(void)
{
	return B8825_CGROUP_CPUSET_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cgroup_cpuset_id_8825 - soft cgroup cpuset controller catalog id.
 *
 * Always returns 5. Catalog id only; does not open cgroupfs or call
 * libc. No parent wires.
 */
uint32_t
gj_cgroup_cpuset_id_8825(void)
{
	(void)NULL;
	return b8825_cpuset_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cgroup_cpuset_id_8825(void)
    __attribute__((alias("gj_cgroup_cpuset_id_8825")));
