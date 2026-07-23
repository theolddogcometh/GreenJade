/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8728: soft plain-fork readiness flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fork_ready_u_8728(uint32_t flags);
 *     - Return 1 if flags look like a plain fork (no CLONE_VM,
 *       CLONE_VFORK, CLONE_THREAD, CLONE_NEWNS, or CLONE_NEWPID bits),
 *       else 0.
 *   uint32_t __gj_fork_ready_u_8728  (alias)
 *   __libcgj_batch8728_marker = "libcgj-batch8728"
 *
 * Exclusive continuum CREATE-ONLY (8721-8730: clone/fork flag id stubs —
 * clone_vm_id, clone_fs_id, clone_files_id, clone_sighand_id,
 * clone_thread_id, clone_newns_id, clone_newpid_id, fork_ready_u,
 * vfork_ready_u, batch_id_8730). Unique gj_fork_ready_u_8728 surface
 * only; no multi-def. Soft pure mask test; does not call fork/clone.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8728_marker[] = "libcgj-batch8728";

/* Soft bits that rule out classic plain-fork semantics. */
#define B8728_CLONE_VM     ((uint32_t)0x00000100u)
#define B8728_CLONE_VFORK  ((uint32_t)0x00004000u)
#define B8728_CLONE_THREAD ((uint32_t)0x00010000u)
#define B8728_CLONE_NEWNS  ((uint32_t)0x00020000u)
#define B8728_CLONE_NEWPID ((uint32_t)0x20000000u)
#define B8728_FORK_BLOCK \
	(B8728_CLONE_VM | B8728_CLONE_VFORK | B8728_CLONE_THREAD | \
	 B8728_CLONE_NEWNS | B8728_CLONE_NEWPID)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8728_fork_ready(uint32_t u32Flags)
{
	return ((u32Flags & B8728_FORK_BLOCK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fork_ready_u_8728 - 1 if flags look plain-fork ready.
 *
 * flags: raw clone / clone3 flags bitmask (signal nibble ignored)
 *
 * Returns 1 when none of the soft fork-blocking share/namespace bits
 * are set. Self-contained; does not call sibling gj_clone_*_id helpers
 * or fork(2). No parent wires.
 */
uint32_t
gj_fork_ready_u_8728(uint32_t u32Flags)
{
	(void)NULL;
	return b8728_fork_ready(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fork_ready_u_8728(uint32_t u32Flags)
    __attribute__((alias("gj_fork_ready_u_8728")));
