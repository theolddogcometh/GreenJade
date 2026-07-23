/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8787: seccomp SECCOMP_RET_ERRNO action id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seccomp_ret_errno_id_8787(void);
 *     - Return SECCOMP_RET_ERRNO action code (0x00050000).
 *   uint32_t __gj_seccomp_ret_errno_id_8787  (alias)
 *   __libcgj_batch8787_marker = "libcgj-batch8787"
 *
 * Exclusive continuum CREATE-ONLY (8781-8790: landlock/seccomp id stubs —
 * landlock_ready_u, landlock_fs_read_id, landlock_fs_write_id,
 * landlock_fs_exec_id, seccomp_ret_allow_id, seccomp_ret_kill_id,
 * seccomp_ret_errno_id, seccomp_mode_filter_id, seccomp_ready_u,
 * batch_id_8790). Unique gj_seccomp_ret_errno_id_8787 surface only; no
 * multi-def. Catalog id only; no seccomp filter install. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8787_marker[] = "libcgj-batch8787";

/* Soft action: SECCOMP_RET_ERRNO (deny + errno in lower 16 bits). */
#define B8787_SECCOMP_RET_ERRNO  ((uint32_t)0x00050000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8787_ret_errno_id(void)
{
	return B8787_SECCOMP_RET_ERRNO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seccomp_ret_errno_id_8787 - soft SECCOMP_RET_ERRNO action id.
 *
 * Always returns 0x00050000. Catalog id only; does not install a
 * seccomp filter. No parent wires.
 */
uint32_t
gj_seccomp_ret_errno_id_8787(void)
{
	(void)NULL;
	return b8787_ret_errno_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seccomp_ret_errno_id_8787(void)
    __attribute__((alias("gj_seccomp_ret_errno_id_8787")));
