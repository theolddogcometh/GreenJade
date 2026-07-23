/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8646: x86_64 syscall number id stub (fstat).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sys_fstat_id_8646(void);
 *     - Returns the traditional x86_64 __NR_fstat number as a uint32 tag (5).
 *       Tag only — does not invoke any syscall.
 *   uint32_t __gj_sys_fstat_id_8646  (alias)
 *   __libcgj_batch8646_marker = "libcgj-batch8646"
 *
 * Exclusive CREATE-ONLY wave (8641-8650): syscall number id stubs for
 * common x86_64 numbers. Does NOT redefine prior gj_sys_* / batch_id_*
 * symbols — avoid multi-def. No parent wires. No syscall invocation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8646_marker[] = "libcgj-batch8646";

/* Traditional x86_64 __NR_fstat. */
#define B8646_SYS_FSTAT_ID  5u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8646_sys_fstat_id(void)
{
	return B8646_SYS_FSTAT_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sys_fstat_id_8646 — report the traditional x86_64 fstat syscall number.
 *
 * Always returns 5 (x86_64 __NR_fstat). Tag only; no syscall is issued.
 * No parent wires.
 */
uint32_t
gj_sys_fstat_id_8646(void)
{
	(void)NULL;
	return b8646_sys_fstat_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sys_fstat_id_8646(void)
    __attribute__((alias("gj_sys_fstat_id_8646")));
