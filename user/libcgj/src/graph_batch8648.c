/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8648: x86_64 syscall number id stub (mprotect).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sys_mprotect_id_8648(void);
 *     - Returns the traditional x86_64 __NR_mprotect number as a uint32 tag (10).
 *       Tag only — does not invoke any syscall.
 *   uint32_t __gj_sys_mprotect_id_8648  (alias)
 *   __libcgj_batch8648_marker = "libcgj-batch8648"
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

const char __libcgj_batch8648_marker[] = "libcgj-batch8648";

/* Traditional x86_64 __NR_mprotect. */
#define B8648_SYS_MPROTECT_ID  10u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8648_sys_mprotect_id(void)
{
	return B8648_SYS_MPROTECT_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sys_mprotect_id_8648 — report the traditional x86_64 mprotect syscall
 * number.
 *
 * Always returns 10 (x86_64 __NR_mprotect). Tag only; no syscall is issued.
 * No parent wires.
 */
uint32_t
gj_sys_mprotect_id_8648(void)
{
	(void)NULL;
	return b8648_sys_mprotect_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sys_mprotect_id_8648(void)
    __attribute__((alias("gj_sys_mprotect_id_8648")));
