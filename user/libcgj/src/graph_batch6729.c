/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6729: clone pthread-ish compound flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clone_threadish_u_6729(uint32_t flags);
 *     - Return 1 if all soft pthread-create clone bits are set
 *       (VM|FS|FILES|SIGHAND|THREAD|SYSVSEM|CHILD_CLEARTID|
 *       PARENT_SETTID), else 0.
 *   uint32_t __gj_clone_threadish_u_6729  (alias)
 *   __libcgj_batch6729_marker = "libcgj-batch6729"
 *
 * Exclusive continuum CREATE-ONLY (6721-6730: clone flags stubs —
 * vm, fs, files, sighand, thread, sysvsem, settls, parent_settid,
 * threadish, batch_id_6730). Unique gj_clone_threadish_u_6729 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6729_marker[] = "libcgj-batch6729";

/* Soft pthread-create clone mask (sched.h / glibc pthread create). */
#define B6729_CLONE_VM            ((uint32_t)0x00000100u)
#define B6729_CLONE_FS            ((uint32_t)0x00000200u)
#define B6729_CLONE_FILES         ((uint32_t)0x00000400u)
#define B6729_CLONE_SIGHAND       ((uint32_t)0x00000800u)
#define B6729_CLONE_THREAD        ((uint32_t)0x00010000u)
#define B6729_CLONE_SYSVSEM       ((uint32_t)0x00040000u)
#define B6729_CLONE_CHILD_CLEARTID ((uint32_t)0x00200000u)
#define B6729_CLONE_PARENT_SETTID ((uint32_t)0x00100000u)
#define B6729_THREADISH_MASK \
	(B6729_CLONE_VM | B6729_CLONE_FS | B6729_CLONE_FILES | \
	 B6729_CLONE_SIGHAND | B6729_CLONE_THREAD | B6729_CLONE_SYSVSEM | \
	 B6729_CLONE_CHILD_CLEARTID | B6729_CLONE_PARENT_SETTID)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6729_clone_threadish(uint32_t u32Flags)
{
	return ((u32Flags & B6729_THREADISH_MASK) == B6729_THREADISH_MASK)
	    ? 1u
	    : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clone_threadish_u_6729 - 1 if full soft pthread-clone mask is set.
 *
 * flags: raw clone / clone3 flags bitmask
 *
 * Returns 1 when every pthread-ish share bit is present. Self-contained;
 * does not call sibling gj_clone_*_u_672x helpers. No parent wires.
 */
uint32_t
gj_clone_threadish_u_6729(uint32_t u32Flags)
{
	(void)NULL;
	return b6729_clone_threadish(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clone_threadish_u_6729(uint32_t u32Flags)
    __attribute__((alias("gj_clone_threadish_u_6729")));
