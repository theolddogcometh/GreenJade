/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6989: kcmp type validity soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_kcmp_type_ok_u_6989(uint32_t kcmp_type);
 *     - Return 1 if kcmp_type is a known kcmp type in [0, 7]
 *       (KCMP_FILE..KCMP_EPOLL_TFD), else 0.
 *   uint32_t __gj_kcmp_type_ok_u_6989  (alias)
 *   __libcgj_batch6989_marker = "libcgj-batch6989"
 *
 * Exclusive continuum CREATE-ONLY (6981-6990: kcmp type stubs —
 * file, vm, files, fs, sighand, io, sysvsem, epoll_tfd, type_ok,
 * batch_id_6990). Unique gj_kcmp_type_ok_u_6989 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6989_marker[] = "libcgj-batch6989";

/* Known kcmp types: KCMP_FILE=0 .. KCMP_EPOLL_TFD=7 (KCMP_TYPES=8). */
#define B6989_KCMP_TYPE_MAX ((uint32_t)7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6989_kcmp_type_ok(uint32_t u32Type)
{
	return (u32Type <= B6989_KCMP_TYPE_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kcmp_type_ok_u_6989 - 1 if kcmp_type is a known kcmp type.
 *
 * kcmp_type: kcmp(2) type argument
 *
 * Returns 1 for types in [0, 7] (FILE..EPOLL_TFD), else 0.
 * Soft range check; no parent wires.
 */
uint32_t
gj_kcmp_type_ok_u_6989(uint32_t u32KcmpType)
{
	(void)NULL;
	return b6989_kcmp_type_ok(u32KcmpType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_kcmp_type_ok_u_6989(uint32_t u32KcmpType)
    __attribute__((alias("gj_kcmp_type_ok_u_6989")));
