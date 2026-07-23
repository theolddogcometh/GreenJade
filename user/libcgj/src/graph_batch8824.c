/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8824: cgroup pids controller id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cgroup_pids_id_8824(void);
 *     - Return soft cgroup pids controller catalog id (4).
 *   uint32_t __gj_cgroup_pids_id_8824  (alias)
 *   __libcgj_batch8824_marker = "libcgj-batch8824"
 *
 * Exclusive continuum CREATE-ONLY (8821-8830: cgroup/controller id
 * stubs — cpu_id, memory_id, io_id, pids_id, cpuset_id, v2_ready_u,
 * path_ok_u, fd_ok_u, ready_u, batch_id_8830). Unique
 * gj_cgroup_pids_id_8824 surface only; no multi-def. Catalog id only;
 * no cgroup syscalls. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8824_marker[] = "libcgj-batch8824";

/* Soft catalog ordinal for the cgroup pids controller. */
#define B8824_CGROUP_PIDS_ID  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8824_pids_id(void)
{
	return B8824_CGROUP_PIDS_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cgroup_pids_id_8824 - soft cgroup pids controller catalog id.
 *
 * Always returns 4. Catalog id only; does not open cgroupfs or call
 * libc. No parent wires.
 */
uint32_t
gj_cgroup_pids_id_8824(void)
{
	(void)NULL;
	return b8824_pids_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cgroup_pids_id_8824(void)
    __attribute__((alias("gj_cgroup_pids_id_8824")));
