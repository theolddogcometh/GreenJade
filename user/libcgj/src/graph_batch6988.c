/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6988: kcmp KCMP_EPOLL_TFD type stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_kcmp_type_epoll_tfd_u_6988(uint32_t kcmp_type);
 *     - Return 1 if kcmp_type is KCMP_EPOLL_TFD (7), else 0.
 *   uint32_t __gj_kcmp_type_epoll_tfd_u_6988  (alias)
 *   __libcgj_batch6988_marker = "libcgj-batch6988"
 *
 * Exclusive continuum CREATE-ONLY (6981-6990: kcmp type stubs —
 * file, vm, files, fs, sighand, io, sysvsem, epoll_tfd, type_ok,
 * batch_id_6990). Unique gj_kcmp_type_epoll_tfd_u_6988 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6988_marker[] = "libcgj-batch6988";

/* KCMP_EPOLL_TFD: compare epoll target file slot (value 7). */
#define B6988_KCMP_EPOLL_TFD ((uint32_t)7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6988_kcmp_type_epoll_tfd(uint32_t u32Type)
{
	return (u32Type == B6988_KCMP_EPOLL_TFD) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kcmp_type_epoll_tfd_u_6988 - 1 if kcmp_type is KCMP_EPOLL_TFD.
 *
 * kcmp_type: kcmp(2) type argument
 *
 * Returns 1 when kcmp_type equals KCMP_EPOLL_TFD (7), else 0.
 * Pure integer compare; no parent wires.
 */
uint32_t
gj_kcmp_type_epoll_tfd_u_6988(uint32_t u32KcmpType)
{
	(void)NULL;
	return b6988_kcmp_type_epoll_tfd(u32KcmpType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_kcmp_type_epoll_tfd_u_6988(uint32_t u32KcmpType)
    __attribute__((alias("gj_kcmp_type_epoll_tfd_u_6988")));
