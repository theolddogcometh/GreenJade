/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8729: soft vfork readiness flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vfork_ready_u_8729(uint32_t flags);
 *     - Return 1 if flags carry the soft vfork pair
 *       (CLONE_VFORK | CLONE_VM), else 0.
 *   uint32_t __gj_vfork_ready_u_8729  (alias)
 *   __libcgj_batch8729_marker = "libcgj-batch8729"
 *
 * Exclusive continuum CREATE-ONLY (8721-8730: clone/fork flag id stubs —
 * clone_vm_id, clone_fs_id, clone_files_id, clone_sighand_id,
 * clone_thread_id, clone_newns_id, clone_newpid_id, fork_ready_u,
 * vfork_ready_u, batch_id_8730). Unique gj_vfork_ready_u_8729 surface
 * only; no multi-def. Soft pure mask test; does not call vfork/clone.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8729_marker[] = "libcgj-batch8729";

/* Soft classic vfork pair (sched.h / Linux vfork path). */
#define B8729_CLONE_VM    ((uint32_t)0x00000100u)
#define B8729_CLONE_VFORK ((uint32_t)0x00004000u)
#define B8729_VFORK_PAIR  (B8729_CLONE_VM | B8729_CLONE_VFORK)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8729_vfork_ready(uint32_t u32Flags)
{
	return ((u32Flags & B8729_VFORK_PAIR) == B8729_VFORK_PAIR) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vfork_ready_u_8729 - 1 if soft vfork flag pair is present.
 *
 * flags: raw clone / clone3 flags bitmask
 *
 * Returns 1 when both CLONE_VFORK and CLONE_VM are set (classic vfork
 * share-until-exec mask). Self-contained; does not call sibling
 * helpers or vfork(2). No parent wires.
 */
uint32_t
gj_vfork_ready_u_8729(uint32_t u32Flags)
{
	(void)NULL;
	return b8729_vfork_ready(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vfork_ready_u_8729(uint32_t u32Flags)
    __attribute__((alias("gj_vfork_ready_u_8729")));
