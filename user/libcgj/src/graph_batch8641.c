/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8641: x86_64 syscall number id stub (read).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sys_read_id_8641(void);
 *     - Returns the traditional x86_64 __NR_read number as a uint32 tag (0).
 *       Tag only — does not invoke any syscall.
 *   uint32_t __gj_sys_read_id_8641  (alias)
 *   __libcgj_batch8641_marker = "libcgj-batch8641"
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

const char __libcgj_batch8641_marker[] = "libcgj-batch8641";

/* Traditional x86_64 __NR_read. */
#define B8641_SYS_READ_ID  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8641_sys_read_id(void)
{
	return B8641_SYS_READ_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sys_read_id_8641 — report the traditional x86_64 read syscall number.
 *
 * Always returns 0 (x86_64 __NR_read). Tag only; no syscall is issued.
 * No parent wires.
 */
uint32_t
gj_sys_read_id_8641(void)
{
	(void)NULL;
	return b8641_sys_read_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sys_read_id_8641(void)
    __attribute__((alias("gj_sys_read_id_8641")));
