/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8788: seccomp SECCOMP_MODE_FILTER mode id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seccomp_mode_filter_id_8788(void);
 *     - Return SECCOMP_MODE_FILTER mode code (2).
 *   uint32_t __gj_seccomp_mode_filter_id_8788  (alias)
 *   __libcgj_batch8788_marker = "libcgj-batch8788"
 *
 * Exclusive continuum CREATE-ONLY (8781-8790: landlock/seccomp id stubs —
 * landlock_ready_u, landlock_fs_read_id, landlock_fs_write_id,
 * landlock_fs_exec_id, seccomp_ret_allow_id, seccomp_ret_kill_id,
 * seccomp_ret_errno_id, seccomp_mode_filter_id, seccomp_ready_u,
 * batch_id_8790). Unique gj_seccomp_mode_filter_id_8788 surface only; no
 * multi-def. Catalog id only; no prctl/seccomp mode set. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8788_marker[] = "libcgj-batch8788";

/* Soft mode: SECCOMP_MODE_FILTER (user-supplied BPF filter). */
#define B8788_SECCOMP_MODE_FILTER  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8788_mode_filter_id(void)
{
	return B8788_SECCOMP_MODE_FILTER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seccomp_mode_filter_id_8788 - soft SECCOMP_MODE_FILTER mode id.
 *
 * Always returns 2. Catalog id only; does not call prctl or seccomp.
 * No parent wires.
 */
uint32_t
gj_seccomp_mode_filter_id_8788(void)
{
	(void)NULL;
	return b8788_mode_filter_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seccomp_mode_filter_id_8788(void)
    __attribute__((alias("gj_seccomp_mode_filter_id_8788")));
