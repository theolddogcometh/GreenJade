/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8644: x86_64 syscall number id stub (close).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sys_close_id_8644(void);
 *     - Returns the traditional x86_64 __NR_close number as a uint32 tag (3).
 *       Tag only — does not invoke any syscall.
 *   uint32_t __gj_sys_close_id_8644  (alias)
 *   __libcgj_batch8644_marker = "libcgj-batch8644"
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

const char __libcgj_batch8644_marker[] = "libcgj-batch8644";

/* Traditional x86_64 __NR_close. */
#define B8644_SYS_CLOSE_ID  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8644_sys_close_id(void)
{
	return B8644_SYS_CLOSE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sys_close_id_8644 — report the traditional x86_64 close syscall number.
 *
 * Always returns 3 (x86_64 __NR_close). Tag only; no syscall is issued.
 * No parent wires.
 */
uint32_t
gj_sys_close_id_8644(void)
{
	(void)NULL;
	return b8644_sys_close_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sys_close_id_8644(void)
    __attribute__((alias("gj_sys_close_id_8644")));
