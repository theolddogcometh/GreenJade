/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6984: kcmp KCMP_FS type stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_kcmp_type_fs_u_6984(uint32_t kcmp_type);
 *     - Return 1 if kcmp_type is KCMP_FS (3), else 0.
 *   uint32_t __gj_kcmp_type_fs_u_6984  (alias)
 *   __libcgj_batch6984_marker = "libcgj-batch6984"
 *
 * Exclusive continuum CREATE-ONLY (6981-6990: kcmp type stubs —
 * file, vm, files, fs, sighand, io, sysvsem, epoll_tfd, type_ok,
 * batch_id_6990). Unique gj_kcmp_type_fs_u_6984 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6984_marker[] = "libcgj-batch6984";

/* KCMP_FS: compare filesystem information (value 3). */
#define B6984_KCMP_FS ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6984_kcmp_type_fs(uint32_t u32Type)
{
	return (u32Type == B6984_KCMP_FS) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kcmp_type_fs_u_6984 - 1 if kcmp_type is KCMP_FS.
 *
 * kcmp_type: kcmp(2) type argument
 *
 * Returns 1 when kcmp_type equals KCMP_FS (3), else 0.
 * Pure integer compare; no parent wires.
 */
uint32_t
gj_kcmp_type_fs_u_6984(uint32_t u32KcmpType)
{
	(void)NULL;
	return b6984_kcmp_type_fs(u32KcmpType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_kcmp_type_fs_u_6984(uint32_t u32KcmpType)
    __attribute__((alias("gj_kcmp_type_fs_u_6984")));
