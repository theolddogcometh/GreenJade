/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8647: x86_64 syscall number id stub (mmap).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sys_mmap_id_8647(void);
 *     - Returns the traditional x86_64 __NR_mmap number as a uint32 tag (9).
 *       Tag only — does not invoke any syscall.
 *   uint32_t __gj_sys_mmap_id_8647  (alias)
 *   __libcgj_batch8647_marker = "libcgj-batch8647"
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

const char __libcgj_batch8647_marker[] = "libcgj-batch8647";

/* Traditional x86_64 __NR_mmap. */
#define B8647_SYS_MMAP_ID  9u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8647_sys_mmap_id(void)
{
	return B8647_SYS_MMAP_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sys_mmap_id_8647 — report the traditional x86_64 mmap syscall number.
 *
 * Always returns 9 (x86_64 __NR_mmap). Tag only; no syscall is issued.
 * No parent wires.
 */
uint32_t
gj_sys_mmap_id_8647(void)
{
	(void)NULL;
	return b8647_sys_mmap_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sys_mmap_id_8647(void)
    __attribute__((alias("gj_sys_mmap_id_8647")));
