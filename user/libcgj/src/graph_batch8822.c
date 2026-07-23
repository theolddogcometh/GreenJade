/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8822: cgroup memory controller id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cgroup_memory_id_8822(void);
 *     - Return soft cgroup memory controller catalog id (2).
 *   uint32_t __gj_cgroup_memory_id_8822  (alias)
 *   __libcgj_batch8822_marker = "libcgj-batch8822"
 *
 * Exclusive continuum CREATE-ONLY (8821-8830: cgroup/controller id
 * stubs — cpu_id, memory_id, io_id, pids_id, cpuset_id, v2_ready_u,
 * path_ok_u, fd_ok_u, ready_u, batch_id_8830). Unique
 * gj_cgroup_memory_id_8822 surface only; no multi-def. Catalog id only;
 * no cgroup syscalls. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8822_marker[] = "libcgj-batch8822";

/* Soft catalog ordinal for the cgroup memory controller. */
#define B8822_CGROUP_MEMORY_ID  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8822_memory_id(void)
{
	return B8822_CGROUP_MEMORY_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cgroup_memory_id_8822 - soft cgroup memory controller catalog id.
 *
 * Always returns 2. Catalog id only; does not open cgroupfs or call
 * libc. No parent wires.
 */
uint32_t
gj_cgroup_memory_id_8822(void)
{
	(void)NULL;
	return b8822_memory_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cgroup_memory_id_8822(void)
    __attribute__((alias("gj_cgroup_memory_id_8822")));
