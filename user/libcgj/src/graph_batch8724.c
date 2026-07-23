/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8724: clone CLONE_SIGHAND flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clone_sighand_id_8724(void);
 *     - Return soft clone-flag catalog id for CLONE_SIGHAND (0x00000800).
 *   uint32_t __gj_clone_sighand_id_8724  (alias)
 *   __libcgj_batch8724_marker = "libcgj-batch8724"
 *
 * Exclusive continuum CREATE-ONLY (8721-8730: clone/fork flag id stubs —
 * clone_vm_id, clone_fs_id, clone_files_id, clone_sighand_id,
 * clone_thread_id, clone_newns_id, clone_newpid_id, fork_ready_u,
 * vfork_ready_u, batch_id_8730). Unique gj_clone_sighand_id_8724 surface
 * only; no multi-def. Soft pure-data catalog only; does not call clone.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8724_marker[] = "libcgj-batch8724";

/* Soft clone-flag catalog: CLONE_SIGHAND share signal handlers (0x00000800). */
#define B8724_CLONE_SIGHAND  ((uint32_t)0x00000800u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8724_clone_sighand_id(void)
{
	return B8724_CLONE_SIGHAND;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clone_sighand_id_8724 - soft CLONE_SIGHAND flag catalog id.
 *
 * Always returns 0x00000800 (CLONE_SIGHAND). Pure data tag; does not
 * share sighand_struct. No parent wires.
 */
uint32_t
gj_clone_sighand_id_8724(void)
{
	(void)NULL;
	return b8724_clone_sighand_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clone_sighand_id_8724(void)
    __attribute__((alias("gj_clone_sighand_id_8724")));
