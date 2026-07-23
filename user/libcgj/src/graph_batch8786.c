/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8786: seccomp SECCOMP_RET_KILL action id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seccomp_ret_kill_id_8786(void);
 *     - Return SECCOMP_RET_KILL / SECCOMP_RET_KILL_THREAD action code
 *       (0x00000000).
 *   uint32_t __gj_seccomp_ret_kill_id_8786  (alias)
 *   __libcgj_batch8786_marker = "libcgj-batch8786"
 *
 * Exclusive continuum CREATE-ONLY (8781-8790: landlock/seccomp id stubs —
 * landlock_ready_u, landlock_fs_read_id, landlock_fs_write_id,
 * landlock_fs_exec_id, seccomp_ret_allow_id, seccomp_ret_kill_id,
 * seccomp_ret_errno_id, seccomp_mode_filter_id, seccomp_ready_u,
 * batch_id_8790). Unique gj_seccomp_ret_kill_id_8786 surface only; no
 * multi-def. Catalog id only; no seccomp filter install. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8786_marker[] = "libcgj-batch8786";

/* Soft action: SECCOMP_RET_KILL / SECCOMP_RET_KILL_THREAD. */
#define B8786_SECCOMP_RET_KILL  ((uint32_t)0x00000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8786_ret_kill_id(void)
{
	return B8786_SECCOMP_RET_KILL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seccomp_ret_kill_id_8786 - soft SECCOMP_RET_KILL action id.
 *
 * Always returns 0x00000000 (KILL_THREAD alias). Catalog id only; does
 * not install a seccomp filter. No parent wires.
 */
uint32_t
gj_seccomp_ret_kill_id_8786(void)
{
	(void)NULL;
	return b8786_ret_kill_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seccomp_ret_kill_id_8786(void)
    __attribute__((alias("gj_seccomp_ret_kill_id_8786")));
