/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8782: landlock FS read-file access id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_landlock_fs_read_id_8782(void);
 *     - Return LANDLOCK_ACCESS_FS_READ_FILE soft bit (1ULL << 2 == 4).
 *   uint32_t __gj_landlock_fs_read_id_8782  (alias)
 *   __libcgj_batch8782_marker = "libcgj-batch8782"
 *
 * Exclusive continuum CREATE-ONLY (8781-8790: landlock/seccomp id stubs —
 * landlock_ready_u, landlock_fs_read_id, landlock_fs_write_id,
 * landlock_fs_exec_id, seccomp_ret_allow_id, seccomp_ret_kill_id,
 * seccomp_ret_errno_id, seccomp_mode_filter_id, seccomp_ready_u,
 * batch_id_8790). Unique gj_landlock_fs_read_id_8782 surface only; no
 * multi-def. Catalog id only; no landlock_add_rule. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8782_marker[] = "libcgj-batch8782";

/* Soft bit: LANDLOCK_ACCESS_FS_READ_FILE (1ULL << 2). */
#define B8782_LL_FS_READ_FILE  ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8782_fs_read_id(void)
{
	return B8782_LL_FS_READ_FILE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_landlock_fs_read_id_8782 - soft LANDLOCK_ACCESS_FS_READ_FILE id.
 *
 * Always returns 4 (bit 2). Catalog id only; does not call landlock
 * syscalls. No parent wires.
 */
uint32_t
gj_landlock_fs_read_id_8782(void)
{
	(void)NULL;
	return b8782_fs_read_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_landlock_fs_read_id_8782(void)
    __attribute__((alias("gj_landlock_fs_read_id_8782")));
