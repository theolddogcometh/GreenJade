/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8790: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8790(void);
 *     - Returns the compile-time graph batch number for this TU (8790).
 *   uint32_t __gj_batch_id_8790  (alias)
 *   __libcgj_batch8790_marker = "libcgj-batch8790"
 *
 * Exclusive continuum CREATE-ONLY (8781-8790: landlock/seccomp id stubs —
 * landlock_ready_u, landlock_fs_read_id, landlock_fs_write_id,
 * landlock_fs_exec_id, seccomp_ret_allow_id, seccomp_ret_kill_id,
 * seccomp_ret_errno_id, seccomp_mode_filter_id, seccomp_ready_u,
 * batch_id_8790). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Soft pure-data catalog
 * only; no landlock/seccomp syscalls. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8790_marker[] = "libcgj-batch8790";

#define B8790_BATCH_ID  8790u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8790_id(void)
{
	return B8790_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8790 - report this TU's graph batch number.
 *
 * Always returns 8790. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8790(void)
{
	(void)NULL;
	return b8790_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8790(void)
    __attribute__((alias("gj_batch_id_8790")));
