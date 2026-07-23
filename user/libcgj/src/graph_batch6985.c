/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6985: kcmp KCMP_SIGHAND type stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_kcmp_type_sighand_u_6985(uint32_t kcmp_type);
 *     - Return 1 if kcmp_type is KCMP_SIGHAND (4), else 0.
 *   uint32_t __gj_kcmp_type_sighand_u_6985  (alias)
 *   __libcgj_batch6985_marker = "libcgj-batch6985"
 *
 * Exclusive continuum CREATE-ONLY (6981-6990: kcmp type stubs —
 * file, vm, files, fs, sighand, io, sysvsem, epoll_tfd, type_ok,
 * batch_id_6990). Unique gj_kcmp_type_sighand_u_6985 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6985_marker[] = "libcgj-batch6985";

/* KCMP_SIGHAND: compare signal dispositions (value 4). */
#define B6985_KCMP_SIGHAND ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6985_kcmp_type_sighand(uint32_t u32Type)
{
	return (u32Type == B6985_KCMP_SIGHAND) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kcmp_type_sighand_u_6985 - 1 if kcmp_type is KCMP_SIGHAND.
 *
 * kcmp_type: kcmp(2) type argument
 *
 * Returns 1 when kcmp_type equals KCMP_SIGHAND (4), else 0.
 * Pure integer compare; no parent wires.
 */
uint32_t
gj_kcmp_type_sighand_u_6985(uint32_t u32KcmpType)
{
	(void)NULL;
	return b6985_kcmp_type_sighand(u32KcmpType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_kcmp_type_sighand_u_6985(uint32_t u32KcmpType)
    __attribute__((alias("gj_kcmp_type_sighand_u_6985")));
