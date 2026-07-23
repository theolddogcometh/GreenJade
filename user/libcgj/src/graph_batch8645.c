/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8645: x86_64 syscall number id stub (stat).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sys_stat_id_8645(void);
 *     - Returns the traditional x86_64 __NR_stat number as a uint32 tag (4).
 *       Tag only — does not invoke any syscall.
 *   uint32_t __gj_sys_stat_id_8645  (alias)
 *   __libcgj_batch8645_marker = "libcgj-batch8645"
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

const char __libcgj_batch8645_marker[] = "libcgj-batch8645";

/* Traditional x86_64 __NR_stat. */
#define B8645_SYS_STAT_ID  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8645_sys_stat_id(void)
{
	return B8645_SYS_STAT_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sys_stat_id_8645 — report the traditional x86_64 stat syscall number.
 *
 * Always returns 4 (x86_64 __NR_stat). Tag only; no syscall is issued.
 * No parent wires.
 */
uint32_t
gj_sys_stat_id_8645(void)
{
	(void)NULL;
	return b8645_sys_stat_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sys_stat_id_8645(void)
    __attribute__((alias("gj_sys_stat_id_8645")));
