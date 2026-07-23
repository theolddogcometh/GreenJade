/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8725: clone CLONE_THREAD flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clone_thread_id_8725(void);
 *     - Return soft clone-flag catalog id for CLONE_THREAD (0x00010000).
 *   uint32_t __gj_clone_thread_id_8725  (alias)
 *   __libcgj_batch8725_marker = "libcgj-batch8725"
 *
 * Exclusive continuum CREATE-ONLY (8721-8730: clone/fork flag id stubs —
 * clone_vm_id, clone_fs_id, clone_files_id, clone_sighand_id,
 * clone_thread_id, clone_newns_id, clone_newpid_id, fork_ready_u,
 * vfork_ready_u, batch_id_8730). Unique gj_clone_thread_id_8725 surface
 * only; no multi-def. Soft pure-data catalog only; does not call clone.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8725_marker[] = "libcgj-batch8725";

/* Soft clone-flag catalog: CLONE_THREAD same thread group (0x00010000). */
#define B8725_CLONE_THREAD  ((uint32_t)0x00010000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8725_clone_thread_id(void)
{
	return B8725_CLONE_THREAD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clone_thread_id_8725 - soft CLONE_THREAD flag catalog id.
 *
 * Always returns 0x00010000 (CLONE_THREAD). Pure data tag; does not join
 * a thread group or create a pthread. No parent wires.
 */
uint32_t
gj_clone_thread_id_8725(void)
{
	(void)NULL;
	return b8725_clone_thread_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clone_thread_id_8725(void)
    __attribute__((alias("gj_clone_thread_id_8725")));
