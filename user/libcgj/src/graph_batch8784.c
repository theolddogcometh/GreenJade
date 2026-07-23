/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8784: landlock FS execute access id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_landlock_fs_exec_id_8784(void);
 *     - Return LANDLOCK_ACCESS_FS_EXECUTE soft bit (1ULL << 0 == 1).
 *   uint32_t __gj_landlock_fs_exec_id_8784  (alias)
 *   __libcgj_batch8784_marker = "libcgj-batch8784"
 *
 * Exclusive continuum CREATE-ONLY (8781-8790: landlock/seccomp id stubs —
 * landlock_ready_u, landlock_fs_read_id, landlock_fs_write_id,
 * landlock_fs_exec_id, seccomp_ret_allow_id, seccomp_ret_kill_id,
 * seccomp_ret_errno_id, seccomp_mode_filter_id, seccomp_ready_u,
 * batch_id_8790). Unique gj_landlock_fs_exec_id_8784 surface only; no
 * multi-def. Catalog id only; no landlock_add_rule. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8784_marker[] = "libcgj-batch8784";

/* Soft bit: LANDLOCK_ACCESS_FS_EXECUTE (1ULL << 0). */
#define B8784_LL_FS_EXECUTE  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8784_fs_exec_id(void)
{
	return B8784_LL_FS_EXECUTE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_landlock_fs_exec_id_8784 - soft LANDLOCK_ACCESS_FS_EXECUTE id.
 *
 * Always returns 1 (bit 0). Catalog id only; does not call landlock
 * syscalls. No parent wires.
 */
uint32_t
gj_landlock_fs_exec_id_8784(void)
{
	(void)NULL;
	return b8784_fs_exec_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_landlock_fs_exec_id_8784(void)
    __attribute__((alias("gj_landlock_fs_exec_id_8784")));
