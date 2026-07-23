/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8830: cgroup controller id continuum batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8830(void);
 *     - Returns the compile-time graph batch number for this TU (8830).
 *   uint32_t __gj_batch_id_8830  (alias)
 *   __libcgj_batch8830_marker = "libcgj-batch8830"
 *
 * Exclusive continuum CREATE-ONLY (8821-8830: cgroup/controller id
 * stubs — cpu_id, memory_id, io_id, pids_id, cpuset_id, v2_ready_u,
 * path_ok_u, fd_ok_u, ready_u, batch_id_8830). Unique
 * gj_batch_id_8830 surface only; no multi-def. Does NOT redefine bare
 * gj_batch_id. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8830_marker[] = "libcgj-batch8830";

#define B8830_BATCH_ID  8830u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8830_id(void)
{
	return B8830_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8830 - report this TU's graph batch number.
 *
 * Always returns 8830. Soft pure-data product tag for the 8821-8830
 * cgroup/controller id stubs continuum closeout. No parent wires.
 */
uint32_t
gj_batch_id_8830(void)
{
	(void)NULL;
	return b8830_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8830(void)
    __attribute__((alias("gj_batch_id_8830")));
