/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8643: x86_64 syscall number id stub (open).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sys_open_id_8643(void);
 *     - Returns the traditional x86_64 __NR_open number as a uint32 tag (2).
 *       Tag only — does not invoke any syscall.
 *   uint32_t __gj_sys_open_id_8643  (alias)
 *   __libcgj_batch8643_marker = "libcgj-batch8643"
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

const char __libcgj_batch8643_marker[] = "libcgj-batch8643";

/* Traditional x86_64 __NR_open. */
#define B8643_SYS_OPEN_ID  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8643_sys_open_id(void)
{
	return B8643_SYS_OPEN_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sys_open_id_8643 — report the traditional x86_64 open syscall number.
 *
 * Always returns 2 (x86_64 __NR_open). Tag only; no syscall is issued.
 * No parent wires.
 */
uint32_t
gj_sys_open_id_8643(void)
{
	(void)NULL;
	return b8643_sys_open_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sys_open_id_8643(void)
    __attribute__((alias("gj_sys_open_id_8643")));
