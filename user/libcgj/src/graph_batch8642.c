/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8642: x86_64 syscall number id stub (write).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sys_write_id_8642(void);
 *     - Returns the traditional x86_64 __NR_write number as a uint32 tag (1).
 *       Tag only — does not invoke any syscall.
 *   uint32_t __gj_sys_write_id_8642  (alias)
 *   __libcgj_batch8642_marker = "libcgj-batch8642"
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

const char __libcgj_batch8642_marker[] = "libcgj-batch8642";

/* Traditional x86_64 __NR_write. */
#define B8642_SYS_WRITE_ID  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8642_sys_write_id(void)
{
	return B8642_SYS_WRITE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sys_write_id_8642 — report the traditional x86_64 write syscall number.
 *
 * Always returns 1 (x86_64 __NR_write). Tag only; no syscall is issued.
 * No parent wires.
 */
uint32_t
gj_sys_write_id_8642(void)
{
	(void)NULL;
	return b8642_sys_write_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sys_write_id_8642(void)
    __attribute__((alias("gj_sys_write_id_8642")));
