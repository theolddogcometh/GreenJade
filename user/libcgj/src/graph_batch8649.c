/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8649: x86_64 syscall number id stub (exit).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sys_exit_id_8649(void);
 *     - Returns the traditional x86_64 __NR_exit number as a uint32 tag (60).
 *       Tag only — does not invoke any syscall.
 *   uint32_t __gj_sys_exit_id_8649  (alias)
 *   __libcgj_batch8649_marker = "libcgj-batch8649"
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

const char __libcgj_batch8649_marker[] = "libcgj-batch8649";

/* Traditional x86_64 __NR_exit. */
#define B8649_SYS_EXIT_ID  60u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8649_sys_exit_id(void)
{
	return B8649_SYS_EXIT_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sys_exit_id_8649 — report the traditional x86_64 exit syscall number.
 *
 * Always returns 60 (x86_64 __NR_exit). Tag only; no syscall is issued.
 * No parent wires.
 */
uint32_t
gj_sys_exit_id_8649(void)
{
	(void)NULL;
	return b8649_sys_exit_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sys_exit_id_8649(void)
    __attribute__((alias("gj_sys_exit_id_8649")));
